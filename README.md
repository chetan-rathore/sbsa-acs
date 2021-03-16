# Base System Architecture - Architecture Compliance Suite


## Base System Architecture
**Base System Architecture** (BSA) specification describes a hardware system architecture based on the Arm 64-bit architecture. System software such as operating systems, hypervisors, and firmware rely on this. It addresses PE features and key aspects of system architecture.

For more information, download the [BSA specification](https://developer.arm.com/documentation/den0094/latest)


## BSA - Architecture Compliance Suite

BSA **Architecture Compliance Suite** (ACS) is a collection of self-checking, portable C-based tests.
This suite includes a set of examples of the invariant behaviors that are provided by the [BSA](https://developer.arm.com/documentation/den0094/latest) specification, so that you can verify if these behaviour have been interpreted correctly.
Most of the tests are executed from UEFI Shell by executing the BSA UEFI shell application.
A few tests are executed by running the BSA ACS Linux application which in turn depends on the BSA ACS Linux kernel module.


## Release details
 - Code quality: REL v1.0
 - The tests are written for version 1.0 of the BSA specification.
 - The compliance suite is not a substitute for design verification.
 - To review the BSA ACS logs, Arm licensees can contact Arm directly through their partner managers.
 - To know about the gaps in the test coverage, see [Testcase checklist](docs/testcase-checklist.md).


## GitHub branch
  - To pick up the release version of the code, checkout the release branch.
  - To get the latest version of the code with bug fixes and new features, use the master branch.

## Additional reading
  - For details on the BSA ACS UEFI Shell Application, see [BSA ACS User Guide](docs/Arm_BSA_Architecture_Compliance_User_Guide.pdf).
  - For details on the design of the BSA ACS, see [Validation Methodology Document](docs/Arm_BSA_Architecture_Compliance_Validation_Methodology.pdf).
  - For information about the test coverage scenarios that are implemented in the current release of ACS and the scenarios that are planned for future releases, see [Testcase checklist](docs/testcase-checklist.md).


## BSA ACS Linux kernel module
To enable the export of a few kernel APIs that are necessary for peripheral, PCIe and IOMMU tests, a Linux kernel module and a kernel patch file are required. These files are available at [syscomp_linux_acs](https://ap-gerrit-1.ap01.arm.com/admin/repos/avk/syscomp_linux_acs).

## Target platforms
  Any AArch64 platform that boots UEFI and Linux OS.

## ACS build steps - UEFI Shell application

### Prebuilt images
Prebuilt images for each release are available in the prebuilt_images folder of the release branch. You can choose to use these images or build your own image by following the steps below. If you choose to use the prebuilt image, see the Test suite execution section below for details on how to run the application.

### Prerequisites
    Before you start the ACS build, ensure that the following requirements are met.

- Any mainstream Linux-based OS distribution running on a x86 or AArch64 machine.
- git clone the edk2-stable202008 tag of [EDK2 tree](https://github.com/tianocore/edk2).
- git clone the [EDK2 port of libc](https://github.com/tianocore/edk2-libc) SHA: 61687168fe02ac4d933a36c9145fdd242ac424d1.
- Install GCC 5.3 or a later toolchain for Linux from [here](https://releases.linaro.org/components/toolchain/binaries/).
- Install the build prerequisite packages to build EDK2.
Note: The details of the packages are beyond the scope of this document.

To start the ACS build for ES, perform the following steps:

1.  cd local_edk2_path
2.  git clone https://github.com/tianocore/edk2-libc
3.  git clone https://github.com/ARM-software/bsa-acs ShellPkg/Application/bsa-acs
4.  Add the following to the [LibraryClasses.common] section in ShellPkg/ShellPkg.dsc

>   Add BsaValLib|ShellPkg/Application/bsa-acs/val/BsaValLib.inf

- If the Base System firmware uses ACPI tables
>   Add BsaPalLib|ShellPkg/Application/bsa-acs/platform/pal_uefi/BsaPalLib.inf

- If the Base System firmware uses Device tree**
>   Add BsaPalLib|ShellPkg/Application/bsa-acs/platform/pal_dt/BsaPalLib.inf
    
5.  Add ShellPkg/Application/bsa-acs/uefi_app/BsaAcs.inf in the [components] section of ShellPkg/ShellPkg.dsc

** For systems which support Device tree, ACS efi application is run on top of efi shell which runs
   on u-boot as efi payload.
   - Below change in edk2 code MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.c is required before compiling for IR system.
>          -Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID **) &gHiiConfigRouting);
>          -ASSERT_EFI_ERROR (Status);
>          +//Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID **) &gHiiConfigRouting);
>          +//ASSERT_EFI_ERROR (Status);

### Linux build environment
If the build environment is Linux, perform the following steps:
1.  export GCC49_AARCH64_PREFIX= GCC5.3 toolchain path pointing to /bin/aarch64-linux-gnu- in case of x86 machine. For an AArch64 build it should point to /usr/bin/
2.  export PACKAGES_PATH= path pointing to edk2-libc
3.  source edksetup.sh
4.  make -C BaseTools/Source/C
5.  source ShellPkg/Application/bsa-acs/tools/scripts/acsbuild.sh

### Windows build environment
If the build environment is Windows, perform the following steps:
1. Set the toolchain path to GCC53 or above.
2. Set the environment for AArch64 EDK2 build.
3. Set the environment for PACKAGES_PATH.
4. Build the BSA shell application.
   For example,
   build -a AARCH64 -t GCC49 -p ShellPkg/ShellPkg.dsc -m
   ShellPkg/Application/bsa-acs/uefi_app/BsaAcs.inf

### Build output

The EFI executable file is generated at <edk2_path>/Build/Shell/DEBUG_GCC49/AARCH64/Bsa.efi


## Test suite execution

The execution of the compliance suite varies depending on the test environment. The below steps assume that the test suite is invoked through the ACS UEFI shell application.

For details about the BSA ACS UEFI Shell application, see [BSA ACS USER Guide](docs/Arm_BSA_Architecture_Compliance_User_Guide.pdf)

### Prerequisites
- If the system supports LPIs (Interrupt ID > 8192) then Firmware should support installation of handler for LPI interrupts.
    - If you are using edk2, change the ArmGic driver in the ArmPkg to support installation of handler for LPIs.
    - Add the following in edk2/ArmPkg/Drivers/ArmGic/GicV3/ArmGicV3Dxe.c
>        - After [#define ARM_GIC_DEFAULT_PRIORITY  0x80]
>          +#define ARM_GIC_MAX_NUM_INTERRUPT 16384
>        - Change this in GicV3DxeInitialize function.
>          -mGicNumInterrupts      = ArmGicGetMaxNumInterrupts (mGicDistributorBase);
>          +mGicNumInterrupts      = ARM_GIC_MAX_NUM_INTERRUPT;

### Post-silicon

On a system where a USB port is available and functional, perform the following steps:

1. Copy 'Bsa.efi' to a USB Flash drive.
2. Plug in the USB flash drive to one of the functional USB ports on the system.
3. Boot the system to UEFI shell.
4. To determine the file system number of the plugged-in USB drive, execute 'map -r' command.
5. Type 'fs<x>' where '<x>' is replaced by the number determined in step 4.
6. To start the compliance tests, run the executable Bsa.efi with the appropriate parameters.
   For details on the parameters, refer to [BSA ACS User Guide](docs/Arm_BSA_Architecture_Compliance_User_Guide.pdf)
7. Copy the UART console output to a log file for analysis and certification.


### Emulation environment with secondary storage
On an emulation environment with secondary storage, perform the following steps:

1. Create an image file which contains the 'Bsa.efi' file. For example:
  - mkfs.vfat -C -n HD0 hda.img 2097152
  - sudo mount -o rw,loop=/dev/loop0,uid=`whoami`,gid=`whoami` hda.img /mnt/bsa
  - cp  "<path to application>/Bsa.efi" /mnt/bsa/
  - sudo umount /mnt/bsa
2. Load the image file to the secondary storage using a backdoor. The steps to load the image file are emulation environment-specific and beyond the scope of this document.
3. Boot the system to UEFI shell.
4. To determine the file system number of the secondary storage, execute 'map -r' command.
5. Type 'fsx' where 'x' is replaced by the number determined in step 4.
6. To start the compliance tests, run the executable Bsa.efi with the appropriate parameters.
   For details on the parameters, see [BSA ACS User Guide](docs/Arm_BSA_Architecture_Compliance_User_Guide.pdf)
7. Copy the UART console output to a log file for analysis and certification.


### Emulation environment without secondary storage

On an emulation platform where secondary storage is not available, perform the following steps:

1. Add the path to 'Bsa.efi' file in the UEFI FD file.
2. Build UEFI image including the UEFI Shell.
3. Boot the system to UEFI shell.
4. Run the executable 'Bsa.efi' to start the compliance tests. For details about the parameters,
   see [BSA ACS User Guide](docs/Arm_BSA_Architecture_Compliance_User_Guide.pdf).
5. Copy the UART console output to a log file for analysis and certification.


## Linux OS-based tests
Certain Peripheral, PCIe and IOMMU tests require Linux operating system with kernel version 4.10 or above.
The procedure to build and run these tests is described in [BSA ACS User Guide](docs/Arm_BSA_Architecture_Compliance_User_Guide.pdf).

## Security implication
The Arm System Ready ACS test suite may run at a higher privilege level. An attacker may utilize these tests to elevate the privilege which can potentially reveal the platform security assets. To prevent the leakage of secure information, Arm strongly recommends that you run the ACS test suite only on development platforms. If it is run on production systems, the system should be scrubbed after running the test suite.


## License
BSA ACS is distributed under Apache v2.0 License.


## Feedback, contributions and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, please send an email to "support-enterprise-acs@arm.com" with details.
 - Arm licensees may contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests. See GitHub documentation on how to raise pull requests.

--------------

*Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.*
