#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <soc/samsung/cal-if.h>
#include <linux/export.h>

u32 exynos7885_big_limit = 2184000;
EXPORT_SYMBOL(exynos7885_big_limit);

#define MAX_BIG_VOLT 1300000

#include "fvmap.h"
#include "cmucal.h"
#include "vclk.h"
#include "ra.h"

#define FVMAP_SIZE		(SZ_8K)
#define STEP_UV			(6250)

void __iomem *map_base;
void __iomem *sram_base;

void __iomem *fvmap_base;
void __iomem *sram_fvmap_base;

static int init_margin_table[MAX_MARGIN_ID] = {0};
static int volt_offset_percent = 0;

static int __init get_all_volt(char *str)
{
	int i, volt;

	get_option(&str, &volt);

	for (i = MARGIN_MIF; i <= MARGIN_G3D; i++) {
		if (init_margin_table[i] == 0)
			init_margin_table[i] = volt;
	}

	return 0;
}
early_param("all", get_all_volt);

static int __init get_mif_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_MIF] = volt;

	return 0;
}
early_param("mif", get_mif_volt);

static int __init get_int_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_INT] = volt;

	return 0;
}
early_param("int", get_int_volt);

static int __init get_big_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_BIG] = volt;

	return 0;
}
early_param("big", get_big_volt);

static int __init get_lit_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_LIT] = volt;

	return 0;
}
early_param("lit", get_lit_volt);

static int __init get_g3d_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_G3D] = volt;

	return 0;
}
early_param("g3d", get_g3d_volt);

static int __init get_fsys_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_FSYS] = volt;

	return 0;
}
early_param("fsys", get_fsys_volt);

static int __init get_intcam_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_INTCAM] = volt;

	return 0;
}
early_param("intcam", get_intcam_volt);

static int __init get_cam_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_CAM] = volt;

	return 0;
}
early_param("cam", get_cam_volt);

static int __init get_disp_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_DISP] = volt;

	return 0;
}
early_param("disp", get_disp_volt);

static int __init get_g3dm_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_G3DM] = volt;

	return 0;
}
early_param("g3dm", get_g3dm_volt);

static int __init get_aud_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_AUD] = volt;

	return 0;
}
early_param("aud", get_aud_volt);

static int __init get_cp_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_CP] = volt;

	return 0;
}
early_param("cp", get_cp_volt);

static int __init get_percent_margin_volt(char *str)
{
	int percent;

	get_option(&str, &percent);
	volt_offset_percent = percent;

	return 0;
}
early_param("volt_offset_percent", get_percent_margin_volt);

int fvmap_set_raw_voltage_table(unsigned int id, int uV)
{
	struct fvmap_header *fvmap_header;
	struct rate_volt_header *fv_table;
	int num_of_lv;
	int idx, i;

	idx = GET_IDX(id);

	fvmap_header = sram_fvmap_base;
	fv_table = sram_fvmap_base + fvmap_header[idx].o_ratevolt;
	num_of_lv = fvmap_header[idx].num_of_lv;

	for (i = 0; i < num_of_lv; i++)
		fv_table->table[i].volt += uV;

	return 0;
}

int fvmap_get_voltage_table(unsigned int id, unsigned int *table)
{
	struct fvmap_header *fvmap_header = fvmap_base;
	struct rate_volt_header *fv_table;
	int idx, i;
	int num_of_lv;

	if (!IS_ACPM_VCLK(id))
		return 0;

	idx = GET_IDX(id);

	fvmap_header = fvmap_base;
	fv_table = fvmap_base + fvmap_header[idx].o_ratevolt;
	num_of_lv = fvmap_header[idx].num_of_lv;

	for (i = 0; i < num_of_lv; i++)
		table[i] = fv_table->table[i].volt;

	return num_of_lv;

}

int fvmap_get_raw_voltage_table(unsigned int id)
{
	struct fvmap_header *fvmap_header;
	struct rate_volt_header *fv_table;
	int idx, i;
	int num_of_lv;
	unsigned int table[20];

	idx = GET_IDX(id);

	fvmap_header = sram_fvmap_base;
	fv_table = sram_fvmap_base + fvmap_header[idx].o_ratevolt;
	num_of_lv = fvmap_header[idx].num_of_lv;

	for (i = 0; i < num_of_lv; i++)
		table[i] = fv_table->table[i].volt;

	for (i = 0; i < num_of_lv; i++)
		printk("dvfs id : %d  %d Khz : %d uv\n", ACPM_VCLK_TYPE | id, fv_table->table[i].rate, table[i]);

	return 0;
}

static void check_percent_margin(struct rate_volt_header *head, unsigned int num_of_lv)
{
	int org_volt;
	int percent_volt;
	int i;

	if (!volt_offset_percent)
		return;

	for (i = 0; i < num_of_lv; i++) {
		org_volt = head->table[i].volt;
		percent_volt = org_volt * volt_offset_percent / 100;
		head->table[i].volt = org_volt + rounddown(percent_volt, STEP_UV);
	}
}

void fvmap_copy_from_sram(void __iomem *map_base,
			  void __iomem *sram_base,
			  unsigned int requested_voltage_change[3])
{
	struct fvmap_header *fvmap_header, *header;
	struct rate_volt_header *old, *new;
	struct clocks *clks;
	struct pll_header *plls;
	struct vclk *vclk;
	struct cmucal_clk *clk_node;
	unsigned int paddr_offset, fvaddr_offset;
	int size, margin;
	int i, j;

	fvmap_header = map_base;
	header = sram_base;

	size = cmucal_get_list_size(ACPM_VCLK_TYPE);

	for (i = 0; i < size; i++) {
		/* load fvmap info */
		fvmap_header[i].dvfs_type = header[i].dvfs_type;
		fvmap_header[i].num_of_lv = header[i].num_of_lv;
		fvmap_header[i].num_of_members = header[i].num_of_members;
		fvmap_header[i].num_of_pll = header[i].num_of_pll;
		fvmap_header[i].num_of_mux = header[i].num_of_mux;
		fvmap_header[i].num_of_div = header[i].num_of_div;
		fvmap_header[i].gearratio = header[i].gearratio;
		fvmap_header[i].init_lv = header[i].init_lv;
		fvmap_header[i].num_of_gate = header[i].num_of_gate;
		fvmap_header[i].reserved[0] = header[i].reserved[0];
		fvmap_header[i].reserved[1] = header[i].reserved[1];
		fvmap_header[i].block_addr[0] = header[i].block_addr[0];
		fvmap_header[i].block_addr[1] = header[i].block_addr[1];
		fvmap_header[i].block_addr[2] = header[i].block_addr[2];
		fvmap_header[i].o_members = header[i].o_members;
		fvmap_header[i].o_ratevolt = header[i].o_ratevolt;
		fvmap_header[i].o_tables = header[i].o_tables;

		vclk = cmucal_get_node(ACPM_VCLK_TYPE | i);
		if (vclk == NULL)
			continue;
		/* While this info is useful to gather initially, don't spam the
		   kernel log with it on single voltage changes */
		if (!requested_voltage_change) {
			pr_info("dvfs_type : %s - id : %x\n",
				vclk->name, fvmap_header[i].dvfs_type);
			pr_info("  num_of_lv      : %d\n", fvmap_header[i].num_of_lv);
			pr_info("  num_of_members : %d\n", fvmap_header[i].num_of_members);
		}

		old = sram_base + fvmap_header[i].o_ratevolt;
		new = map_base + fvmap_header[i].o_ratevolt;

		check_percent_margin(old, fvmap_header[i].num_of_lv);

		margin = init_margin_table[vclk->margin_id];
		if (margin)
			cal_dfs_set_volt_margin(i | ACPM_VCLK_TYPE, margin);

		for (j = 0; j < fvmap_header[i].num_of_lv; j++) {
			/* If a voltage change has been requested,
			   update the appropriate voltage table with it */
			if (requested_voltage_change)
				if ((fvmap_header[i].dvfs_type == requested_voltage_change[0]) &&
				    (old->table[j].rate == requested_voltage_change[1]))
					old->table[j].volt = requested_voltage_change[2];

			new->table[j].rate = old->table[j].rate;
			new->table[j].volt = old->table[j].volt;

			if (!requested_voltage_change)
				pr_info("  lv : [%7d], volt = %d uV (%d %%) \n",
					new->table[j].rate, new->table[j].volt,
					volt_offset_percent);
		}

		for (j = 0; j < fvmap_header[i].num_of_pll; j++) {
			clks = sram_base + fvmap_header[i].o_members;
			plls = sram_base + clks->addr[j];
			clk_node = cmucal_get_node(vclk->list[j]);
			if (clk_node == NULL)
				continue;
			paddr_offset = clk_node->paddr & 0xFFFF;
			fvaddr_offset = plls->addr & 0xFFFF;
			if (paddr_offset == fvaddr_offset)
				continue;

			clk_node->paddr += fvaddr_offset - paddr_offset;
			clk_node->pll_con0 += fvaddr_offset - paddr_offset;
			if (clk_node->pll_con1)
				clk_node->pll_con1 += fvaddr_offset - paddr_offset;
		}
	}
}

void fvmap_change_voltage(unsigned int dvfs_id,
			  unsigned int freq, unsigned int voltage) {
	/* Will be set with the passed info for a voltage change, then
	   passed to fvmap_copy_from_sram() to perform the actual change */
	unsigned int requested_voltage_change[3];

	/* The voltage regulators work in steps of 6250 uV, so all
	   voltages must be divisible by 6250. */
	if ((voltage % 6250) != 0) {
		pr_info("%s: dvfs id %x: invalid voltage change requested "
			"for freq %u kHz - Voltage %u uV invalid as not "
			"divisible by 6250 uV - Rounding down\n",
			__func__, dvfs_id, freq, voltage);
		voltage -= voltage % 6250;
	}

	/* Performing the voltage change */
	requested_voltage_change[0] = dvfs_id;
	requested_voltage_change[1] = freq;
	requested_voltage_change[2] = voltage;
	fvmap_copy_from_sram(map_base, sram_base, requested_voltage_change);
	pr_info("%s: dvfs id %x: changed voltage for freq"
		" %u kHz to %u uV successfully\n",
		__func__, dvfs_id, freq, voltage);
}

#define FVMAP_DUMP_NAME "fvmap_dump"

static u8 fvmap_dump_buf[FVMAP_SIZE];

static ssize_t fvmap_dump_read(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	memcpy_fromio(fvmap_dump_buf, sram_fvmap_base, FVMAP_SIZE);

	return simple_read_from_buffer(buf, count, ppos, fvmap_dump_buf, FVMAP_SIZE);
}

static const struct file_operations fvmap_dump_fops = {
	.owner  = THIS_MODULE,
	.read   = fvmap_dump_read,
	.llseek = default_llseek,
};

inline void fvmap_auto_calc_big_exynos7885(u32 offset)
{
    volatile struct rate_volt *rv_array;
    u32 volt_2184, volt_2080;
    u32 base_step;
    u32 acceleration = STEP_UV;
    u32 current_step;

    u32 raw_2288, raw_2392, raw_2496;
    u32 fin_2288, fin_2392, fin_2496;

    rv_array = (volatile struct rate_volt *)((uintptr_t)sram_fvmap_base + offset);

    volt_2184 = rv_array[3].volt;
    volt_2080 = rv_array[4].volt;

    base_step = volt_2184 - volt_2080;

    if (base_step == 0 || base_step > 100000) base_step = 10 * STEP_UV;

    current_step = base_step + acceleration;
    raw_2288 = volt_2184 + current_step;

    current_step += acceleration;
    raw_2392 = raw_2288 + current_step;

    current_step += acceleration;
    raw_2496 = raw_2392 + current_step;

    if (raw_2288 <= MAX_BIG_VOLT) exynos7885_big_limit = 2288000;
    if (raw_2392 <= MAX_BIG_VOLT) exynos7885_big_limit = 2392000;
    if (raw_2496 <= MAX_BIG_VOLT) exynos7885_big_limit = 2496000;

    fin_2288 = (raw_2288 > MAX_BIG_VOLT) ? MAX_BIG_VOLT : raw_2288;
    fin_2392 = (raw_2392 > MAX_BIG_VOLT) ? MAX_BIG_VOLT : raw_2392;
    fin_2496 = (raw_2496 > MAX_BIG_VOLT) ? MAX_BIG_VOLT : raw_2496;

    rv_array[2].rate = 2288000;
    rv_array[2].volt = fin_2288;

    rv_array[1].rate = 2392000;
    rv_array[1].volt = fin_2392;

    rv_array[0].rate = 2496000;
    rv_array[0].volt = fin_2496;
}

inline void fvmap_override_exynos7885(u32 offset, u32 index, u32 rate, u32 increase_volt)
{
        volatile struct rate_volt *rv_array = (volatile struct rate_volt *)((uintptr_t)sram_fvmap_base + offset);
        volatile struct rate_volt *rv_target = &rv_array[index];

	rv_target->volt += increase_volt;
        rv_target->rate = rate;
}

int fvmap_init(void __iomem *_sram_base)
{
	sram_base = _sram_base;
	map_base = kzalloc(FVMAP_SIZE, GFP_KERNEL);

	fvmap_base = map_base;
	sram_fvmap_base = sram_base;

	fvmap_auto_calc_big_exynos7885(764);

	pr_info("%s: fvmap initialized %pK\n", __func__, sram_base);
	fvmap_copy_from_sram(map_base, sram_base, NULL);

	proc_create(FVMAP_DUMP_NAME, 0444, NULL, &fvmap_dump_fops);

	return 0;
}
