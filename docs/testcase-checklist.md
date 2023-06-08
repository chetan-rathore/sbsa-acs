# SBSA ACS Testcase checklist

## Test Number mapped to SBSA specification section

  - Also, indicates the test coverage.

| Test Number  | Component  | Test Description                        | Required for ACPI systems | Required for Device tree systems     | Required for Baremetal systems| Verified by ACS | Test Enviroment |
|--------------|------------|-----------------------------------------|------|-----------------|-------------------|-----------------|-----------------|
| 1            | PE         | All PE should be architecturally same   | Yes  | No              | Yes               | Yes             | UEFI App        |
| 2            | PE         | PE(s) must implement SIMD extensions    | No   | Yes             | Yes               | No              | Linux App       |

## License
Arm SBSA ACS is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2018-2021, Arm Limited and Contributors. All rights reserved.*
