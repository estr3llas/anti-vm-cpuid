#pragma once

#include <stdio.h>
#include <windows.h>
#include <stdint.h>

/*
	Extended version of HardwareFeatures (added more fields), mostly of them is just miscelaneous and aren't useful against 
 	malware devoloping/sandbox detection. 
	Note that the function void GetHardwareFeatures(PHARDWARE_FEATURES hardwarefeatures); doesnt check if the actual 
 	environment is being virtualized first, it just queries the values as if it is being virtualized.
	
	Important: This code is unfinished, still figuring out some ways to handle HypervisorLevel and incompatibilty between 
 	different Windows versions (OS 10/OS 11).
*/

typedef struct HardwareFeatures {
	int APICOverlayAssist;                      // EAX[0]
	int MSRBitmaps;                             // EAX[1]
	int ArchitecturalPerformanceCounters;       // EAX[2]
	int SLAT;                                   // EAX[3]
	int DMARemapping;                           // EAX[4]
	int InterruptRemapping;                     // EAX[5]
	int MemoryPatrolScrubber;                   // EAX[6]
	int DMAProtection;                          // EAX[7]
	int HPET;                                   // EAX[8]
	int VolatileSyntheticTimers;                // EAX[9]
	//int HypervisorLevel;                      // EAX[10] - Uncomment if needed
	int PhysicalDestinationMode;                // EAX[14]
	int HardwareMemoryZeroing;                  // EAX[16]
	int UnrestrictedGuest;                      // EAX[17]
	int ResourceMonitoring;                     // EAX[19]
	int GuestVirtualPMU;                        // EAX[20]
	int GuestVirtualLBR;                        // EAX[21]
	int GuestVirtualIPT;                        // EAX[22]
	int APICEmulation;                          // EAX[23]
	int ACPIWDAT;                               // EAX[24]
} HARDWARE_FEATURES, *PHARDWARE_FEATURES;


void GetHardwareFeatures(PHARDWARE_FEATURES hardwarefeatures);

int main(int argc, char* argv[]) {

	PHARDWARE_FEATURES hardwarefeatures = (PHARDWARE_FEATURES)calloc(1, sizeof(HARDWARE_FEATURES));

	if (hardwarefeatures == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return EXIT_FAILURE;
	}

	GetHardwareFeatures(hardwarefeatures);

	printf("APIC Overlay Assist value: %d\n", hardwarefeatures->APICOverlayAssist);
	printf("MSR Bitmaps value: %d\n", hardwarefeatures->MSRBitmaps);
	printf("Architectural Performance Counters value: %d\n", hardwarefeatures->ArchitecturalPerformanceCounters);
	printf("SLAT value: %d\n", hardwarefeatures->SLAT);
	printf("DMA Remapping value: %d\n", hardwarefeatures->DMARemapping);
	printf("Interrupt Remapping value: %d\n", hardwarefeatures->InterruptRemapping);
	printf("Memory Patrol Scrubber value: %d\n", hardwarefeatures->MemoryPatrolScrubber);
	printf("DMA Protection value: %d\n", hardwarefeatures->DMAProtection);
	printf("HPET value: %d\n", hardwarefeatures->HPET);
	printf("Volatile Synthetic Timers value: %d\n", hardwarefeatures->VolatileSyntheticTimers);
	// HypervisorLevel: Not implemented yet->
	printf("Physical Destination Mode value: %d\n", hardwarefeatures->PhysicalDestinationMode);
	printf("Hardware Memory Zeroing value: %d\n", hardwarefeatures->HardwareMemoryZeroing);
	printf("Unrestricted Guest value: %d\n", hardwarefeatures->UnrestrictedGuest);
	printf("Resource Monitoring value: %d\n", hardwarefeatures->ResourceMonitoring);
	printf("Guest Virtual PMU value: %d\n", hardwarefeatures->GuestVirtualPMU);
	printf("Guest Virtual LBR value: %d\n", hardwarefeatures->GuestVirtualLBR);
	printf("Guest Virtual IPT value: %d\n", hardwarefeatures->GuestVirtualIPT);
	printf("APIC Emulation value: %d\n", hardwarefeatures->APICEmulation);
	printf("ACPI WDAT value: %d\n", hardwarefeatures->ACPIWDAT);

	free(hardwarefeatures);

	return EXIT_SUCCESS;
}

void GetHardwareFeatures(PHARDWARE_FEATURES hardwarefeatures) {

	__asm {
		mov eax, 0x40000000
		cpuid

		mov ebx, dword ptr[hardwarefeatures]
		test ebx, ebx
		jz done

		cmp eax, 0x40000006
		jge at_least_six

		mov byte ptr[ebx + HARDWARE_FEATURES.SLAT], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.InterruptRemapping], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.DMAProtection], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.PhysicalDestinationMode], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.UnrestrictedGuest], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.ResourceMonitoring], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.ACPIWDAT], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.APICOverlayAssist], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.MSRBitmaps], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.ArchitecturalPerformanceCounters], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.MemoryPatrolScrubber], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.DMAProtection], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.HPET], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.VolatileSyntheticTimers], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.GuestVirtualPMU], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.GuestVirtualLBR], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.GuestVirtualIPT], 0
		mov byte ptr[ebx + HARDWARE_FEATURES.APICEmulation], 0

		jmp done


		at_least_six :
		xor eax, eax
			mov eax, 0x40000006
			cpuid

			test eax, 0x4
			jnz SLAT_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.SLAT], 0

			SLAT_supported :
		mov byte ptr[ebx + HARDWARE_FEATURES.SLAT], 1

			test eax, 0x20
			jnz InterruptRemapping_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.InterruptRemapping], 0

			InterruptRemapping_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.InterruptRemapping], 1

			test eax, 0x80
			jnz DMAProtection_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.DMAProtection], 0

			DMAProtection_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.DMAProtection], 1

			test eax, 0x4000
			jnz PhysicalDestinationMode_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.PhysicalDestinationMode], 0

			PhysicalDestinationMode_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.PhysicalDestinationMode], 1

			test eax, 0x20000
			jnz UnrestrictedGuest_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.UnrestrictedGuest], 0

			UnrestrictedGuest_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.UnrestrictedGuest], 1

			test eax, 0x80000
			jnz ResourceMonitoring_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.ResourceMonitoring], 0

			ResourceMonitoring_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.ResourceMonitoring], 1

			test eax, 0x1000000
			jnz ACPIWDAT_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.ACPIWDAT], 0

			ACPIWDAT_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.ACPIWDAT], 1

			test eax, 0x1
			jnz APICOverlayAssist_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.APICOverlayAssist], 0

			APICOverlayAssist_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.APICOverlayAssist], 1

			test eax, 0x2
			jnz MSRBitmaps_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.MSRBitmaps], 0

			MSRBitmaps_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.MSRBitmaps], 1

			test eax, 0x4
			jnz ArchitecturalPerformanceCounters_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.ArchitecturalPerformanceCounters], 0

			ArchitecturalPerformanceCounters_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.ArchitecturalPerformanceCounters], 1

			test eax, 0x8
			jnz DMARemapping_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.DMAProtection], 0

			DMARemapping_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.DMAProtection], 1

			test eax, 0x40
			jnz MemoryPatrolScrubber_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.MemoryPatrolScrubber], 0

			MemoryPatrolScrubber_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.MemoryPatrolScrubber], 1

			test eax, 0x100
			jnz HPET_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.HPET], 0

			HPET_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.HPET], 1

			test eax, 0x200
			jnz VolatileSyntheticTimers_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.VolatileSyntheticTimers], 0

			VolatileSyntheticTimers_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.VolatileSyntheticTimers], 1

			test eax, 0x10000
			jnz HardwareMemoryZeroing_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.HardwareMemoryZeroing], 0

			HardwareMemoryZeroing_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.HardwareMemoryZeroing], 1

			test eax, 0x100000
			jnz GuestVirtualPMU_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.GuestVirtualPMU], 0

			GuestVirtualPMU_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.GuestVirtualPMU], 1

			test eax, 0x200000
			jnz GuestVirtualLBR_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.GuestVirtualLBR], 0

			GuestVirtualLBR_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.GuestVirtualLBR], 1

			test eax, 0x400000
			jnz GuestVirtualIPT_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.GuestVirtualIPT], 0

			GuestVirtualIPT_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.GuestVirtualIPT], 1

			test eax, 0x800000
			jnz APICEmulation_supported
			mov byte ptr[ebx + HARDWARE_FEATURES.APICEmulation], 0

			APICEmulation_supported :
			mov byte ptr[ebx + HARDWARE_FEATURES.APICEmulation], 1


			done :
	};
}
