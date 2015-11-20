tar  --exclude-from qcapatch.ignore  -jcvf  k22fsh.tar.bz2 `git diff -w --diff-filter=MA --name-only FSLMQXOS_4_1_0_GA qca4002_3_3_x`
