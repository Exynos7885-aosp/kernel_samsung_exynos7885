from kernelbuild_common import KernelBuild, logging
from kernelbuild_common.compiler import CompilerClang
from argparse import ArgumentParser
from pathlib import Path


class EurekaKernelBuild(KernelBuild):
    def __init__(self):
        super().__init__(
            "Eureka", arch="arm64", kernelType="Image", anykernelDir=Path("AnyKernel3")
        )

    def initArgParser(self) -> ArgumentParser:
        parser = super().initArgParser()
        parser.add_argument(
            "--target", type=str, required=True, help="Target device (a10/a20/...)"
        )
        parser.add_argument(
            "--no-ksu",
            action="store_true",
            help="Don't include KernelSU support in kernel",
        )
        return parser

    def verifyArgs(self):
        supplist = ["a10", "a20", "a20e", "a30", "a30s", "a40", "m20", "jackpotlte"]
        if not self.args.target in supplist:
            logging.error(
                f"Invalid target '{self.args.target}'. Supported: {', '.join(supplist)}"
            )
            return False
        return True

    def buildDefconfigList(self) -> "list[str]":
        args = self.args
        defconfigs = ["exynos7885_defconfig", f"{args.target}.config"]
        if args.no_ksu:
            defconfigs.append("noksu.config")
        return defconfigs

    def additionalMakeArgs(self) -> "list[str]":
        return CompilerClang.ADDITIONALARGS_LLVM_FULL

    def preBuildInfo(self) -> "dict[str, str]":
        args = self.args
        return {
            "TARGET_DEVICE": args.target,
            "TARGET_INCLUDES_KSU": not args.no_ksu,
        }

    def zipName(self, name: str, time: str):
        return f"{name}_{self.args.target}_{time}.zip"

    def anykernelFiles(self) -> "list[str]":
        return ["version"]


def main():
    b = EurekaKernelBuild()
    b.build()


if __name__ == "__main__":
    main()
