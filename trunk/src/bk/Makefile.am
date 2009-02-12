h_sources = bkAdd.h bkCache.h bkDelete.h bkError.h bkExtract.h bkGet.h bkIoWrappers.h bkLink.h bkMangle.h bkMisc.h bkPath.h bkRead7x.h bkRead.h bkSet.h bkSort.h bkTime.h bkWrite7x.h bkWrite.h bkInternal.h bk.h
cc_sources = bkAdd.c bkCache.c bkDelete.c bkError.c bkExtract.c bkGet.c bkIoWrappers.c bkLink.c bkMangle.c bkMisc.c bkPath.c bkRead7x.c bkRead.c bkSet.c bkSort.c bkTime.c bkWrite7x.c bkWrite.c

library_includedir = $(includedir)
library_include_HEADERS = $(h_sources)

noinst_LTLIBRARIES = libbk.la
libbk_la_SOURCES = $(h_sources) $(cc_sources) 
