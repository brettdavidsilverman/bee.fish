mod_database.la: mod_database.slo
	$(SH_LINK) -rpath $(libexecdir) -module -avoid-version  mod_database.lo
DISTCLEAN_TARGETS = modules.mk
shared =  mod_database.la
