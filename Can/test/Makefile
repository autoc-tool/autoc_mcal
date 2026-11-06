# Host-side regression tests for the EcuBus JSON-RPC transport.
# Not part of the NuttX image; build and run on the development host.

CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pthread -DCAN_RPC_HOST_TEST
INCLUDES := -Istubs -I../inc -I../host
SRCS := test_can_rpc_transport.c ../src/Can_rpc.c ../host/can_rpc_nx_host.c
OUT ?= /tmp/test_can_rpc_transport

.PHONY: test clean

test: $(OUT)
	$(OUT)

$(OUT): $(SRCS) stubs/Std_Types.h stubs/comstack_types.h stubs/Can_Cfg.h stubs/Can_PBcfg.h ../inc/Can.h ../inc/Can_rpc.h ../host/can_rpc_host.h
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUT) $(SRCS)

clean:
	rm -f $(OUT)
