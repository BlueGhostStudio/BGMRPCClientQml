TEMPLATE = subdirs

SUBDIRS += \
    BGMRPCCommon \
    server \
    client

message($$val_escape(OUT_PWD))
system(cp $$PWD/*.sh $$val_escape(OUT_PWD))
