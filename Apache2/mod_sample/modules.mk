mod_sample.la: mod_sample.slo
	$(SH_LINK) -rpath $(libexecdir) -module -avoid-version  mod_sample.lo
DISTCLEAN_TARGETS = modules.mk
shared =  mod_sample.la
