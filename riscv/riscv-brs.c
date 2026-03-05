// SPDX-License-Identifier: GPL-2.0-only
/*
 * SBI verification
 *
 * Copyright (C) 2026, Xiang W <wangxiang@iscas.ac.cn>
 */
#include <libcflat.h>
#include <stdlib.h>
#include <asm/sbi.h>
#include <asm/isa.h>
#include <devicetree.h>
#include <libfdt/libfdt.h>
#include <acpi.h>

static bool chk_sbi_010(void)
{
	struct sbiret ret;
	ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_GET_SPEC_VERSION, 0, 0, 0, 0, 0, 0);
	if (ret.error)
		return false;
	if (ret.value & (1 << 31))
		return false;
	if (ret.value < (2 << 24))
		return false;
	return true;
}

static bool chk_sbi_020(void)
{
	struct sbiret ret;
	ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_PROBE_EXT, SBI_EXT_HSM, 0, 0, 0, 0, 0);
	if (ret.error)
		return false;
	return true;
}

static bool chk_sbi_030(void)
{
	struct sbiret ret;
	if (has_ext("sstc"))
		return true;
	ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_PROBE_EXT, SBI_EXT_TIME, 0, 0, 0, 0, 0);
	if (ret.error)
		return false;
	return true;
}

static bool chk_sbi_040(void)
{
	struct sbiret ret;
	if (dt_available()) {
		if (fdt_node_offset_by_compatible(dt_fdt(), -1, "riscv,imsics") > 0)
			return true;
	} else
		assert_msg(false, "ACPI not available");
	
	ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_PROBE_EXT, SBI_EXT_IPI, 0, 0, 0, 0, 0);
	if (ret.error)
		return false;
	return true;
}

static bool chk_sbi_050(void)
{
	struct sbiret ret;
	if (dt_available()) {
		if (fdt_node_offset_by_compatible(dt_fdt(), -1, "riscv,imsics") > 0)
			return true;
	} else
		assert_msg(false, "ACPI not available");

	ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_PROBE_EXT, SBI_EXT_RFENCE, 0, 0, 0, 0, 0);
	if (ret.error)
		return false;
	return true;
}
static bool chk_sbi_060(void)
{
	struct sbiret ret;
	if (has_ext("sscsrind") && has_ext("ssccfg"))
		return true;
	ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_PROBE_EXT, SBI_EXT_PMU, 0, 0, 0, 0, 0);
	if (ret.error)
		return false;
	return true;
}

static bool chk_sbi_070(void)
{
#ifdef CONFIG_EFI
	struct sbiret ret;
	struct spcr_descriptor *spcr = find_acpi_table_addr(SPCR_SIGNATURE);
	if (spcr->interface_type != 0x15)
		return true;
	ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_PROBE_EXT, SBI_EXT_DBCN, 0, 0, 0, 0, 0);
	if (ret.error)
		return false;
#endif
	return true;
}


int main(int argc, char **argv)
{
	report_prefix_push("riscv-brs");

	report(chk_sbi_010(), "SBI_010");
	report(chk_sbi_020(), "SBI_020");
	report(chk_sbi_030(), "SBI_030");
	report(chk_sbi_040(), "SBI_040");
	report(chk_sbi_050(), "SBI_050");
	report(chk_sbi_060(), "SBI_060");
	report(chk_sbi_070(), "SBI_070");

	return report_summary();
}
