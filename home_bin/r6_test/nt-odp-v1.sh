#!/bin/bash
#FW='/ocz/users/asharma/projects/R6/evt_sbm/asharma_2014_04_30/Release/mission/FUF/bw-fw-mission.fuf'
#FW='../../../fw/trunk/*.fuf'
FW='../../../fw/asharma/*.fuf'
CHANNEL=0x0000FFFF
TGT=0xFF #make this 0xF0 for IDT board
BLK_OFF=0
BLK_START=0
BLK_RD=16
BLK_CNT=16
PAGE_OFF=0
PAGE_CNT=256
VFY=0
i=1

EN_FW_DOWNLOAD=0
EN_LOGGING=0
LOG_FILE=""
EN_LIMIT=0
LIMIT=2000
RUN_IBBSCAN=0
RUN_NAND_RESET=0
RUN_READ_ID=0
RUN_EWR=0
RUN_LOADGEN=0
RUN_SI=0

usage(){
	echo 'usage: $0 -d -f filename -c limit -b -r -i -e -l -s -v'
	echo '-d = enable downloading of firmware'
	echo '-f = enable logging to <filename>'
	echo '-c = enable erase, write and read of <limit> banks'
	echo '-b = ibbscan'
	echo '-r = nand-reset'
	echo '-i = read-id'
	echo '-e = erase, write and read'
	echo '-v = verbose'
	echo '-l = loadgen'
	echo '-s = signal integrity'
	exit 1
}

while getopts "dbrievslf:c:" o; do
	case "${o}" in
		d)
			EN_FW_DOWNLOAD=1
			;;
		f)
			EN_LOGGING=1
			LOG_FILE=${OPTARG}
			;;
		c)
			EN_LIMIT=1
			LIMIT=${OPTARG}
			;;
		b)
			RUN_IBBSCAN=1
			;;
		r)
			RUN_NAND_RESET=1;
			;;
		i)
			RUN_READ_ID=1;
			;;
		e)
			RUN_EWR=1;
			;;
		v)
			set -x;
			;;
		l)
			RUN_LOADGEN=1;
			;;
		s)
			RUN_SI=1;
			;;
		*)
			usage
			;;
	esac
done
shift $((OPTIND-1))



if [ $EN_FW_DOWNLOAD == '1' ]
then
    sudo nbc sbl-restart
    sudo nbc host-boot $FW
    sleep 5
    exit 1
fi

if [ $EN_LOGGING == '1' ]
then
	exec &> >(tee -a $LOG_FILE)
fi

lim=$LIMIT

if [ $RUN_NAND_RESET == '1' ]
then
	while [[ $i -le 127 ]]; #reset all banks
	do
		nbc smb-vsc nand-reset $i
		i=$((i+1))
	done
fi


if [ $RUN_READ_ID == '1' ]
then
	i=0
	while [[ $i -le 127 ]]; #reset all banks
	do
		nbc smb-vsc read-id $i
		i=$((i+1))
	done
fi

if [ $RUN_IBBSCAN == '1' ]
then
	nbc smb-vsc ibbscan 256
fi

if [ $RUN_EWR == '1' ]
then
	i=0
	while [[ $i -le ${lim} ]]; # 30s/loop
	do
		echo "$i: $(date): "
		BLK_OFF=$(($i*16))
		BLK_OFF=$(($BLK_OFF%2000))
		BLK_CNT=$(($i*16))
		BLK_CNT=$(($BLK_CNT%2000))

		nbc smb-vsc erase $CHANNEL $TGT $BLK_OFF $BLK_CNT
		nbc smb-vsc write $CHANNEL $TGT $BLK_OFF $BLK_CNT $PAGE_OFF $PAGE_CNT
		nbc smb-vsc read $CHANNEL $TGT $BLK_OFF $BLK_CNT $PAGE_OFF $PAGE_CNT
		echo ""

		i=$((i+1))
	done

	echo "$i: $(date): " nbc smb-vsc erase $CHANNEL $TGT $BLK_OFF $BLK_CNT
	nbc smb-vsc erase $CHANNEL $TGT $BLK_OFF $BLK_CNT
	###echo nbc smb-show-log StringTable.gz 9 10
	###nbc smb-show-log StringTable.gz 9 10
fi

#This does not do much. Ignore it
if [ $RUN_LOADGEN == '1' ]
then
		nbc smb-vsc loadgen $CHANNEL $TGT 0 $lim
		nbc smb-vsc loadgen $CHANNEL $TGT 1 $lim
fi

if [ $RUN_SI == '1' ]
then
	nbc smb-vsc nfisi-check $CHANNEL $TGT $lim
fi



