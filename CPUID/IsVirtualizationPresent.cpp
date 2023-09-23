#pragma once

#include <stdio.h>
#include <windows.h>
#include <inttypes.h>

BOOL IsVirtualizationPresent();

int main(int argc, char* argv[]) {

	BOOL beingVirtualized = IsVirtualizationPresent();

	beingVirtualized ? puts("[+] Virtualization detected!") : puts("[-] No virtualization detected.");

	return EXIT_SUCCESS;
}

BOOL IsVirtualizationPresent() {

	BOOL beingVirtualized;
		
	__asm {
		mov eax, 1
		cpuid

		test ecx, 0x80000000
		jnz is_virtualized

		mov beingVirtualized, 0
		jmp done

	
	is_virtualized:
		mov beingVirtualized, 1
		jmp done

	done:
	};

	if (beingVirtualized)
		return TRUE;

	return FALSE;
}