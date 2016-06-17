cd ~/Desktop/badge-code/

rm -f part_strings.h

header_file=$(mdls -name kMDItemFSName -name kMDItemDateAdded -raw ~/Downloads/*.h | xargs -0 -I {} echo {} | sed 'N;s/\n/ /' | sort | tail -1 | awk '{print $NF}')

echo $header_file

echo $header_file | xargs -I{} cp ~/Downloads/{} ./part_strings.h

/Applications/SimpleIDE.app/Contents/propeller-gcc/bin/propeller-elf-gcc -I . -L . -I ~/Documents/SimpleIDE/Learn/Simple\ Libraries/Utility/libsimpletools -L ~/Documents/SimpleIDE/Learn/Simple\ Libraries/Utility/libsimpletools/cmm/ -I ~/Documents/SimpleIDE/Learn/Simple\ Libraries/TextDevices/libsimpletext -L ~/Documents/SimpleIDE/Learn/Simple\ Libraries/TextDevices/libsimpletext/cmm/ -I ~/Documents/SimpleIDE/Learn/Simple\ Libraries/Protocol/libsimplei2c -L ~/Documents/SimpleIDE/Learn/Simple\ Libraries/Protocol/libsimplei2c/cmm/ -I ~/Desktop/badge-code/libbadgetools -L ~/Desktop/badge-code/libbadgetools/cmm/ -o cmm/participant-code.elf -Os -mcmm -m32bit-doubles -fno-exceptions -std=c99 participant-code.c -lm -lsimpletools -lsimpletext -lsimplei2c -lbadgetools -lm -lsimpletools -lsimpletext -lsimplei2c -lm -lsimpletools -lsimpletext -lm -lsimpletools -lm

/Applications/SimpleIDE.app/Contents/propeller-gcc/bin/propeller-load -s cmm/participant-code.elf

/Applications/SimpleIDE.app/Contents/propeller-gcc/bin/propeller-elf-objdump -h cmm/participant-code.elf

result=$(ls /dev | grep "cu.usbserial-" | cut -c14-)

/Applications/SimpleIDE.app/Contents/propeller-gcc/bin/propeller-load -Dreset=dtr -I /Applications/SimpleIDE.app/Contents/MacOS/../propeller-gcc/propeller-load/ -b ACTIVITYBOARD cmm/participant-code.elf -e -r -p /dev/cu.usbserial-$result
