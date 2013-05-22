/* Shared library add-on to iptables to add MARK target support. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <iptables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
/* For 64bit kernel / 32bit userspace */
#include "../include/linux/netfilter_ipv4/ipt_MARK.h"

/* Function which prints out usage message. */
static void
help(void)
{
	printf(
"MARK target v%s options:\n"
"  --set-mark value                   Set nfmark value\n"
"  --and-mark value                   Binary AND the nfmark with value\n"
"  --or-mark  value                   Binary OR  the nfmark with value\n"
"\n",
IPTABLES_VERSION);
}

static struct option opts[] = {
	{ "set-mark", 1, 0, '1' },
	{ "and-mark", 1, 0, '2' },
	{ "or-mark", 1, 0, '3' },
	{ 0 }
};

static struct option mark_tg_opts[] = {
	{.name = "set-xmark", .has_arg = 1, .val = 'X'},
	{.name = "set-mark",  .has_arg = 1, .val = '='},
	{.name = "and-mark",  .has_arg = 1, .val = '&'},
	{.name = "or-mark",   .has_arg = 1, .val = '|'},
	{.name = "xor-mark",  .has_arg = 1, .val = '^'},
	{ 0 }
};

static void
mark_tg_help(void)
{
	printf(
"MARK target options:\n"
"  --set-xmark value[/mask]  Clear bits in mask and XOR value into nfmark\n"
"  --set-mark value[/mask]   Clear bits in mask and OR value into nfmark\n"
"  --and-mark bits           Binary AND the nfmark with bits\n"
"  --or-mark bits            Binary OR the nfmark with bits\n"
"  --xor-mask bits           Binary XOR the nfmark with bits\n"
"\n");
}

/* Initialize the target. */
static void
init(struct ipt_entry_target *t, unsigned int *nfcache)
{
}

/* Function which parses command options; returns true if it
   ate an option */
static int
parse_v0(int c, char **argv, int invert, unsigned int *flags,
	 const struct ipt_entry *entry,
	 struct ipt_entry_target **target)
{
	struct ipt_mark_target_info *markinfo
		= (struct ipt_mark_target_info *)(*target)->data;

	switch (c) {
	case '1':
#ifdef KERNEL_64_USERSPACE_32
		if (string_to_number_ll(optarg, 0, 0, 
				     &markinfo->mark))
#else
		if (string_to_number_l(optarg, 0, 0, 
				     &markinfo->mark))
#endif
			exit_error(PARAMETER_PROBLEM, "Bad MARK value `%s'", optarg);
		if (*flags)
			exit_error(PARAMETER_PROBLEM,
			           "MARK target: Can't specify --set-mark twice");
		*flags = 1;
		break;
	case '2':
		exit_error(PARAMETER_PROBLEM,
			   "MARK target: kernel too old for --and-mark");
	case '3':
		exit_error(PARAMETER_PROBLEM,
			   "MARK target: kernel too old for --or-mark");
	default:
		return 0;
	}

	return 1;
}

static void
final_check(unsigned int flags)
{
	if (!flags)
		exit_error(PARAMETER_PROBLEM,
		           "MARK target: Parameter --set/and/or-mark"
			   " is required");
}

/* Function which parses command options; returns true if it
   ate an option */
static int
parse_v1(int c, char **argv, int invert, unsigned int *flags,
	 const struct ipt_entry *entry,
	 struct ipt_entry_target **target)
{
	struct ipt_mark_target_info_v1 *markinfo
		= (struct ipt_mark_target_info_v1 *)(*target)->data;

	switch (c) {
	case '1':
	        markinfo->mode = IPT_MARK_SET;
		break;
	case '2':
	        markinfo->mode = IPT_MARK_AND;
		break;
	case '3':
	        markinfo->mode = IPT_MARK_OR;
		break;
	default:
		return 0;
	}

#ifdef KERNEL_64_USERSPACE_32
	if (string_to_number_ll(optarg, 0, 0,  &markinfo->mark))
#else
	if (string_to_number_l(optarg, 0, 0, &markinfo->mark))
#endif
		exit_error(PARAMETER_PROBLEM, "Bad MARK value `%s'", optarg);

	if (*flags)
		exit_error(PARAMETER_PROBLEM,
			   "MARK target: Can't specify --set-mark twice");

	*flags = 1;
	return 1;
}

static int
mark_tg_parse(int c, char **argv, int invert, unsigned int *flags,
              const struct ipt_entry *entry,
              struct ipt_entry_target **target)
{
	struct xt_mark_tginfo2 *info = (void *)(*target)->data;
	unsigned int value, mask = UINT32_MAX;
	char *end;

	switch (c) {
	case 'X': /* --set-xmark */
	case '=': /* --set-mark */
		value = strtoul(optarg, &end, 0);
                if (*end == '/')
                        mask = strtoul(end+1, &end, 0);
		if (*end != '\0' || end == optarg)
			exit_error(PARAMETER_PROBLEM, "Bad MARK value `%s'", optarg);

		info->mark = value;
		info->mask = mask;

		if (c == '=')
			info->mask = value | mask;
		break;

	case '&': /* --and-mark */
		mask = strtoul(optarg, &end, 0);
		if (*end != '\0' || end == optarg)
			exit_error(PARAMETER_PROBLEM, "Bad MARK value '%s'", optarg);

		info->mark = 0;
		info->mask = ~mask;
		break;

	case '|': /* --or-mark */
		value = strtoul(optarg, &end, 0);
		if (*end != '\0' || end == optarg)
			exit_error(PARAMETER_PROBLEM, "Bad MARK value '%s'", optarg);

		info->mark = value;
		info->mask = value;
		break;

	case '^': /* --xor-mark */
		value = strtoul(optarg, &end, 0);
		if (*end != '\0' || end == optarg)
			exit_error(PARAMETER_PROBLEM, "Bad MARK value '%s'", optarg);

		info->mark = value;
		info->mask = 0;
		break;

	default:
		return 0;
	}

	if (*flags)
		exit_error(PARAMETER_PROBLEM, "At most one action is possible");
	if (invert)
		exit_error(PARAMETER_PROBLEM, "--set-xmark/--set-mark cannot be inverted");

	*flags = 1;
	return 1;
}

static void
mark_tg_check(unsigned int flags)
{
	if (flags == 0)
		exit_error(PARAMETER_PROBLEM, "MARK: One of the --set-xmark, "
		           "--{and,or,xor,set}-mark options is required");
}

#ifdef KERNEL_64_USERSPACE_32
static void
print_mark(unsigned long long mark)
{
	printf("0x%llx ", mark);
}
#else
static void
print_mark(unsigned long mark)
{
	printf("0x%lx ", mark);
}
#endif

/* Prints out the targinfo. */
static void
print_v0(const struct ipt_ip *ip,
	 const struct ipt_entry_target *target,
	 int numeric)
{
	const struct ipt_mark_target_info *markinfo =
		(const struct ipt_mark_target_info *)target->data;
	printf("MARK set ");
	print_mark(markinfo->mark);
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void
save_v0(const struct ipt_ip *ip, const struct ipt_entry_target *target)
{
	const struct ipt_mark_target_info *markinfo =
		(const struct ipt_mark_target_info *)target->data;

	printf("--set-mark ");
	print_mark(markinfo->mark);
}

/* Prints out the targinfo. */
static void
print_v1(const struct ipt_ip *ip,
	 const struct ipt_entry_target *target,
	 int numeric)
{
	const struct ipt_mark_target_info_v1 *markinfo =
		(const struct ipt_mark_target_info_v1 *)target->data;

	switch (markinfo->mode) {
	case IPT_MARK_SET:
		printf("MARK set ");
		break;
	case IPT_MARK_AND:
		printf("MARK and ");
		break;
	case IPT_MARK_OR: 
		printf("MARK or ");
		break;
	}
	print_mark(markinfo->mark);
}

static void
mark_tg_print(const struct ipt_ip *ip, const struct ipt_entry_target *target,
                          int numeric)
{
	const struct xt_mark_tginfo2 *info = (const void *)target->data;

	if (info->mark == 0)
		printf("MARK and 0x%x ", (unsigned int)(u_int32_t)~info->mask);
	else if (info->mark == info->mask)
		printf("MARK or 0x%x ", info->mark);
	else if (info->mask == 0)
		printf("MARK xor 0x%x ", info->mark);
	else if (info->mask == 0xffffffffU)
		printf("MARK set 0x%x ", info->mark);
	else
		printf("MARK xset 0x%x/0x%x ", info->mark, info->mask);
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void
save_v1(const struct ipt_ip *ip, const struct ipt_entry_target *target)
{
	const struct ipt_mark_target_info_v1 *markinfo =
		(const struct ipt_mark_target_info_v1 *)target->data;

	switch (markinfo->mode) {
	case IPT_MARK_SET:
		printf("--set-mark ");
		break;
	case IPT_MARK_AND:
		printf("--and-mark ");
		break;
	case IPT_MARK_OR: 
		printf("--or-mark ");
		break;
	}
	print_mark(markinfo->mark);
}

static void
mark_tg_save(const struct ipt_ip *ip, const struct ipt_entry_target *target)
{
	const struct xt_mark_tginfo2 *info = (const void *)target->data;

	printf("--set-xmark 0x%x/0x%x ", info->mark, info->mask);
}

static
struct iptables_target mark_v0 = {
	.next		= NULL,
	.name		= "MARK",
	.version	= IPTABLES_VERSION,
	.revision	= 0,
	.size		= IPT_ALIGN(sizeof(struct ipt_mark_target_info)),
	.userspacesize	= IPT_ALIGN(sizeof(struct ipt_mark_target_info)),
	.help		= &help,
	.init		= &init,
	.parse		= &parse_v0,
	.final_check	= &final_check,
	.print		= &print_v0,
	.save		= &save_v0,
	.extra_opts	= opts
};

static
struct iptables_target mark_v1 = {
	.next		= NULL,
	.name		= "MARK",
	.version	= IPTABLES_VERSION,
	.revision	= 1,
	.size		= IPT_ALIGN(sizeof(struct ipt_mark_target_info_v1)),
	.userspacesize	= IPT_ALIGN(sizeof(struct ipt_mark_target_info_v1)),
	.help		= &help,
	.init		= &init,
	.parse		= &parse_v1,
	.final_check	= &final_check,
	.print		= &print_v1,
	.save		= &save_v1,
	.extra_opts	= opts
};

static
struct iptables_target mark_v2 ={
	.next		= NULL,
	.name		= "MARK",
	.version	= IPTABLES_VERSION,
	.revision	= 2,
	.size		= IPT_ALIGN(sizeof(struct xt_mark_tginfo2)),
	.userspacesize	= IPT_ALIGN(sizeof(struct xt_mark_tginfo2)),
	.help		= &mark_tg_help,
	.parse		= &mark_tg_parse,
	.final_check	= &mark_tg_check,
	.print		= &mark_tg_print,
	.save		= &mark_tg_save,
	.extra_opts	= mark_tg_opts,
};

void ipt_MARK_init(void)
{
	register_target(&mark_v0);
	register_target(&mark_v1);
	register_target(&mark_v2);
}
