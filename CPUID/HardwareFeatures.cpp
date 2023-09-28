#include <stdio.h>
#include <windows.h>
#include <stdint.h>

typedef struct HardwareFeatures {
	BOOL SLAT;						//EAX[3]
	BOOL InterruptRemapping;		//EAX[5]
	BOOL DMAProtection;				//EAX[7]
	uint32_t HypervisorLevel;		//((unsigned long)EAX >> 10) & 0x0F
	BOOL PhysicalDestinationMode;	//EAX[14]
	BOOL UnrestrictedGuest;			//EAX[17]
	BOOL ResourceMonitoring;		//EAX[19]
	BOOL ACPIWDAT;					//EAX[24]
} HardwareFeatures;

HardwareFeatures GetHardwareFeatures();

int main(int argc, char* argv[]) {

	struct HardwareFeatures hardwarefeatures;

	hardwarefeatures = GetHardwareFeatures();

	printf("SLAT value: %d\n", hardwarefeatures.SLAT);
	printf("InterruptRemapping value: %d\n", hardwarefeatures.InterruptRemapping);
	printf("DMAProtection value: %d\n", hardwarefeatures.DMAProtection);
	printf("HypervisorLevel value: %d\n", hardwarefeatures.HypervisorLevel);
	printf("PhysicalDestinationMode value: %d\n", hardwarefeatures.PhysicalDestinationMode);
	printf("UnrestrictedGuest value: %d\n", hardwarefeatures.UnrestrictedGuest);
	printf("ResourceMonitoring value: %d\n", hardwarefeatures.ResourceMonitoring);
	printf("ACPIWDAT value: %d\n", hardwarefeatures.ACPIWDAT);

	return EXIT_SUCCESS;
}

HardwareFeatures GetHardwareFeatures() {

	BOOL beingVirtualized;
	BOOL hardwareFeatures_enabled;

	struct HardwareFeatures hardwarefeatures;

	hardwarefeatures.SLAT = FALSE;
	hardwarefeatures.InterruptRemapping = FALSE;
	hardwarefeatures.DMAProtection = FALSE;
	hardwarefeatures.HypervisorLevel = 0;
	hardwarefeatures.PhysicalDestinationMode = FALSE;
	hardwarefeatures.UnrestrictedGuest = FALSE;
	hardwarefeatures.ResourceMonitoring = FALSE;
	hardwarefeatures.ACPIWDAT = FALSE;

	__asm {
		mov eax, 1
		cpuid

		test ecx, 0x80000000
		jnz is_virtualized

		mov beingVirtualized, 0
		jmp done


		is_virtualized :
			mov beingVirtualized, 1

			xor eax, eax
			mov eax, 0x40000000
			cpuid

			cmp eax, 0x40000006
			jge at_least_six
			mov hardwareFeatures_enabled, 0
			mov hardwarefeatures.SLAT, 0
			mov hardwarefeatures.InterruptRemapping, 0
			mov hardwarefeatures.DMAProtection, 0
			mov hardwarefeatures.PhysicalDestinationMode, 0
			mov hardwarefeatures.UnrestrictedGuest, 0
			mov hardwarefeatures.ResourceMonitoring, 0
			mov hardwarefeatures.ACPIWDAT, 0
			mov hardwarefeatures.HypervisorLevel, 0
			jmp done


		at_least_six :
			mov hardwareFeatures_enabled, 1
			xor eax, eax
			mov eax, 0x40000006
			cpuid

			test eax, 0x4
			jnz SLAT_supported
			mov hardwarefeatures.SLAT, 0

		SLAT_supported :
			mov hardwarefeatures.SLAT, 1

			test eax, 0x20
			jnz InterruptRemapping_supported
			mov hardwarefeatures.InterruptRemapping, 0

		InterruptRemapping_supported :
			mov hardwarefeatures.InterruptRemapping, 1

			test eax, 0x80
			jnz DMAProtection_supported
			mov hardwarefeatures.DMAProtection, 0

		DMAProtection_supported :
			mov hardwarefeatures.DMAProtection, 1

			test eax, 0x4000
			jnz PhysicalDestinationMode_supported
			mov hardwarefeatures.PhysicalDestinationMode, 0

		PhysicalDestinationMode_supported :
			mov hardwarefeatures.PhysicalDestinationMode, 1

			test eax, 0x20000
			jnz UnrestrictedGuest_supported
			mov hardwarefeatures.UnrestrictedGuest, 0

		UnrestrictedGuest_supported :
			mov hardwarefeatures.UnrestrictedGuest, 1

			test eax, 0x80000
			jnz ResourceMonitoring_supported
			mov hardwarefeatures.ResourceMonitoring, 0

		ResourceMonitoring_supported :
			mov hardwarefeatures.ResourceMonitoring, 1

			test eax, 0x1000000
			jnz ACPIWDAT_supported
			mov hardwarefeatures.ACPIWDAT, 0

		ACPIWDAT_supported :
			mov hardwarefeatures.ACPIWDAT, 1

			shr eax, 10
			and eax, 0x0F
			mov hardwarefeatures.HypervisorLevel, eax


			done :
	};

	if (beingVirtualized) {
		puts("[+] Virtualization Detected! Now checking if it is possible to query Hardware Features...");
		if (hardwareFeatures_enabled)
			return hardwarefeatures;
		puts("[-] It is not possible to query Hardware Features.");
		return hardwarefeatures;
	}

	puts("[-] The environment is not an virtualized environment");

	return hardwarefeatures;
}
