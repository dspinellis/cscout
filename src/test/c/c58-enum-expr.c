enum {
        BTF_KIND_UNKN           = 0,    /* Unknown      */
        BTF_KIND_INT            = 1,    /* Integer      */

        NR_BTF_KINDS,
        BTF_KIND_MAX            = NR_BTF_KINDS - 1,
};

static const char * const btf_kind_str[NR_BTF_KINDS] = {
        [BTF_KIND_UNKN]         = "UNKNOWN",
        [BTF_KIND_INT]          = "INT"
};
