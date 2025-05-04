dtb-$(CONFIG_EXYNOS7885_USE_DEVICE_TREE_OVERLAY) += exynos/dtbo/exynos7885.dtb

dtbo-$(CONFIG_EXYNOS7884B_A10_EUR_OPEN) += \
	exynos/dtbo/exynos7884B-a10_eur_open_00.dtbo \
	exynos/dtbo/exynos7884B-a10_eur_open_01.dtbo
dtbo-$(CONFIG_EXYNOS7884B_A20_SWA_OPEN) += \
	exynos/dtbo/exynos7884B-a20_swa_open_00.dtbo \
	exynos/dtbo/exynos7884B-a20_swa_open_01.dtbo \
	exynos/dtbo/exynos7884B-a20_swa_open_03.dtbo
dtbo-$(CONFIG_EXYNOS7884B_A20E_EUR_OPEN) += \
	exynos/dtbo/exynos7884B-a20e_eur_open_00.dtbo \
	exynos/dtbo/exynos7884B-a20e_eur_open_03.dtbo
dtbo-$(CONFIG_EXYNOS7904_A30_SWA_OPEN) += \
	exynos/dtbo/exynos7904-a30_swa_open_00.dtbo \
	exynos/dtbo/exynos7904-a30_swa_open_01.dtbo \
	exynos/dtbo/exynos7904-a30_swa_open_02.dtbo
dtbo-$(CONFIG_EXYNOS7904_A30S_EUR_OPEN) += \
	exynos/dtbo/exynos7904-a30s_eur_open_00.dtbo \
	exynos/dtbo/exynos7904-a30s_eur_open_01.dtbo \
	exynos/dtbo/exynos7904-a30s_eur_open_02.dtbo \
	exynos/dtbo/exynos7904-a30s_eur_open_03.dtbo
dtbo-$(CONFIG_EXYNOS7904_A40_EUR_OPEN) += \
	exynos/dtbo/exynos7904-a40_eur_open_00.dtbo \
	exynos/dtbo/exynos7904-a40_eur_open_01.dtbo
dtb-$(CONFIG_EXYNOS7904_M20LTE_SWA_OPEN) += exynos/exynos7904-m20lte_swa_open_04.dtb
dtb-$(CONFIG_EXYNOS7904_M30LTE_SWA_OPEN) += \
	exynos/exynos7904-m30lte_swa_open_00.dtb \
	exynos/exynos7904-m30lte_swa_open_01.dtb \
	exynos/exynos7904-m30lte_swa_open_03.dtb \
	exynos/exynos7904-m30lte_swa_open_04.dtb
dtb-$(CONFIG_EXYNOS7885_JACKPOTLTE_EUR_OPEN) += \
	exynos/exynos7885-jackpotlte_eur_open_00.dtb \
	exynos/exynos7885-jackpotlte_eur_open_01.dtb \
	exynos/exynos7885-jackpotlte_eur_open_02.dtb \
	exynos/exynos7885-jackpotlte_eur_open_03.dtb \
	exynos/exynos7885-jackpotlte_eur_open_04.dtb \
	exynos/exynos7885-jackpotlte_eur_open_05.dtb \
	exynos/exynos7885-jackpotlte_eur_open_06.dtb \
	exynos/exynos7885-jackpotlte_eur_open_07.dtb
dtb-$(CONFIG_EXYNOS7885_JACKPOT2LTE_EUR_OPEN) += \
	exynos/exynos7885-jackpot2lte_eur_open_00.dtb \
	exynos/exynos7885-jackpot2lte_eur_open_01.dtb \
	exynos/exynos7885-jackpot2lte_eur_open_02.dtb \
	exynos/exynos7885-jackpot2lte_eur_open_03.dtb \
	exynos/exynos7885-jackpot2lte_eur_open_04.dtb \
	exynos/exynos7885-jackpot2lte_eur_open_05.dtb \
	exynos/exynos7885-jackpot2lte_eur_open_06.dtb \
	exynos/exynos7885-jackpot2lte_eur_open_07.dtb

dts-dirs += altera
dts-dirs += amd
dts-dirs += apm
dts-dirs += arm
dts-dirs += broadcom
dts-dirs += cavium
#dts-dirs += exynos
dts-dirs += freescale
dts-dirs += hisilicon
dts-dirs += marvell
dts-dirs += mediatek
dts-dirs += qcom
dts-dirs += rockchip
dts-dirs += sprd
dts-dirs += xilinx

subdir-y	:= $(dts-dirs)

dtstree		:= $(srctree)/$(src)

dtb-$(CONFIG_OF_ALL_DTBS) := $(patsubst $(dtstree)/%.dts,%.dtb, $(foreach d,$(dts-dirs), $(wildcard $(dtstree)/$(d)/*.dts)))

always		:= $(dtb-y) $(dtbo-y)
clean-files	:= dts/*.dtb *.dtb*
