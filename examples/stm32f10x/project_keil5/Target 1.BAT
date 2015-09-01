SET PATH=d:\Keil_v5\ARM\ARMCC\Bin;C:\ProgramData\Oracle\Java\javapath;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files (x86)\Intel\OpenCL SDK\2.0\bin\x86;C:\Program Files (x86)\Intel\OpenCL SDK\2.0\bin\x64;C:\Program Files\Microsoft SQL Server\110\Tools\Binn\;D:\Program Files (x86)\IVI Foundation\VISA\WinNT\Bin\;C:\Program Files\IVI Foundation\VISA\Win64\Bin\;D:\Program Files (x86)\IVI Foundation\VISA\WinNT\Bin;C:\Program Files\TortoiseSVN\bin;C:\Program Files\TortoiseGit\bin
SET CPU_TYPE=STM32F103ZE
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=Target 1
SET CPU_CLOCK=0x00B71B00
"d:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\core_cm3.__i"
"d:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\output\startup_stm32f10x_hd._ia"
"d:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\system_stm32f10x.__i"
"d:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\misc.__i"
"d:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_gpio.__i"
"d:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_rcc.__i"
"d:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\stm32f10x_tim.__i"
"d:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\nftask.__i"
"d:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\nframe_config.__i"
"d:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\nftask_example.__i"
"d:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\output\example.lnp"
"d:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\output\example.axf" --i32combined --output ".\output\example.hex"
