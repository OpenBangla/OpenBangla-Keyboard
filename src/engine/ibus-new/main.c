#include "stdio.h"
#include "libriti.h"

int main(int argc, char* argv[]) {
    printf("\nWorking!!\n");
    RitiContext *ctx = riti_context_new();
    riti_context_free(ctx);
	return 0;
}