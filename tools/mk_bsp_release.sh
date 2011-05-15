#!/bin/bash
ROOT=`pwd`
LSDKNAME=STAR8100-LSDK
BASE=STAR8100-LSDK-base.tar.bz2
#TOOLCHAIN_BZ2=STAR8100-LSDK/tools/arm-uclibc-3.4.6_kc.tar.bz2
TOOLCHAIN_BZ2=$ROOT/$LSDKNAME/tools/arm-uclibc-3.4.6_kc.tar.bz2
TARGET=STAR8100-LSDK-6.7.2-rcX.tar.bz2
TARGET_BIN=STAR8100-LSDK-bin-6.7.2-rcX.tar.bz2
SVNREV=SVNREV
SVNAUTH=
procedure(){
	createopt||return 1

	svnexport||return 1 

	build||return 1 
	
	release $ROOT||return 1
}
createopt(){
	echo === Buildind base directory...
	cd ${ROOT}
	if [ -e ${LSDKNAME} ] ; then echo ${ROOT}/${LSDKNAME} exist ... ;rename "${LSDKNAME}"; fi
	#tar xjf $BASE || return 1
	#mkdir${LSDKNAME} 
	return 0
}

checksvnrev(){
	echo ====== Checking svn rev $1...
	svn info $1 $SVNAUTH > /tmp/infolog ||return 1
	prj_rev=`cat /tmp/infolog | grep "^Revision: [0-9]*"|cut -c 11-`  
	dir_rev=`cat /tmp/infolog|grep "^Last Changed Rev: [0-9]*"|cut -c 19-`
	if [ $prj_rev -ne $dir_rev ] ; then
		echo "Warning: prj_rev\($prj_rev\) doesn\'t match the dir_rev\($dir_rev\) of $1"
		printf "\tHave you forgotten to merge current rev to trunk?\n"
		if  ! yes_or_no "\tDo you want to proceed? (yY/nN):" ;then
			exit 1
		fi
	fi

}
exportsvn(){

	echo === Exporting $1...
	if [ "$3" = "y" ] || [ "$3" = "Y" ] || [ "$3" = "yes" ]; then
		checksvnrev "$1"
	fi
	printf "\texporting...\n"
	svn export $1 $2 > /tmp/exportlog ||return 1
	cat /tmp/exportlog | grep "Exported revision" |sed 's/Exported revision \([0-9]*\)./\1/g'> $2/$SVNREV 

}
svnexport(){
	cd $ROOT ||return 1
	exportsvn "svn://star.kuso.com.tw/str8100-lsdk/lsdk" ${LSDKNAME} "no"

	cd $ROOT/$LSDKNAME/apps ||return 1
	exportsvn "svn://star.kuso.com.tw/str8100-lsdk/apps-pool/samba-3.0.22-star" "samba-3.0.22-star" "no"

	cp -vf $ROOT/arm-uclibc-3.4.6_kc.tar.bz2 $ROOT/$LSDKNAME/tools/

	echo === Extracting toolchain...
	cd $ROOT/${LSDKNAME}/tools ||return 1
	tar xjf ${TOOLCHAIN_BZ2}

	cd $ROOT/${LSDKNAME}/boot ||return 1
	exportsvn "svn://star.kuso.com.tw/bootloader/trunk/u-boot-1.1.4" "u-boot-1.1.4" "yes"


	cd $ROOT/${LSDKNAME}/kernels ||return 1
	exportsvn "svn://star.kuso.com.tw/linux-2.6.16/trunk/linux-2.6.16-star" "linux-2.6.16-star" "yes"
	ln -s linux-2.6.16-star linux

	cp $ROOT/$0  $ROOT/${LSDKNAME}/tools/ -vf
	return 0

}
build(){
	echo === Compiling the whole project...
	#if [ "$ROOT" != "/" ] ; then echo To be compiled correctly, project required to be located at / instead of $ROOT, stopped.; return 1;fi
	cd $ROOT/${LSDKNAME} ||return 1
	export PATH=$ROOT/${LSDKNAME}/tools/arm-uclibc-3.4.6/bin:$PATH
	./mk_star_board_image.sh|| return 1 
	#make clean
	#make distclean -C kernels/linux-2.6.16-star
	#make distclean -C u-boot-1.1.4

	return 0
}
release(){
	cd $ROOT 
	#cd $1 
	echo === Removing .svn files...
	#find ${ROOT}/${LSDKNAME} -name .svn -exec rm {} -rf \; 2>/dev/null
	find -name .svn -exec rm {} -rf \; 2>/dev/null
	echo === Starting to tar LSDK-bin...
	tar cjf $TARGET_BIN ${LSDKNAME}/output/*
	rm ${LSDKNAME}/output/* -fv
	cd $ROOT/${LSDKNAME} ||return 1
	make clean
        make distclean -C kernels/linux-2.6.16-star
	make distclean -C boot/u-boot-1.1.4
	echo === Starting to tar LSDK...
	cd $ROOT
	tar cjf $TARGET ${LSDKNAME}
	echo Done.
}
usage(){
	echo Usage:
	echo 1. $0 create [\<username\> \<password\>]
	echo 2. $0 svnexport
	echo 3. $0 release
	echo 4. $0 build 
}
yes_or_no(){
	while true ; do
		printf "$1"
		read -n 1 yesno
		printf "\n"
		if [ "$yesno" = "y" ] || [ "$yesno" = "Y" ] ; then
		       return 0
		fi
	 	if [ "$yesno" = "n" ] || [ "$yesno" = "N" ] ; then
			return 1
		fi
	done

}
rename(){
	a=1;
	while [ -e "$1.$a" ] ; do a=$(($a+1));done;
	echo \ rename $1 to $1.$a
	mv "$1" "$1.$a"
}
main(){

#	if [ ! -e $BASE ] ; then echo base file $BASE not exist ... ;return 1; fi
	#if [ ! -e $TOOLCHAIN_BZ2 ] ; then echo toolchain $TOOLCHAIN_BZ2 not exist ... ;return 1; fi
	if [ -e "$TARGET" ] ; then echo $TARGET exist ... ;rename "$TARGET"; fi
	if [ -e "$TARGET_BIN" ] ; then echo $TARGET_BIN exist ... ;rename "$TARGET_BIN"; fi
	if [ "x$1" != "x" ] ; then 
		SVNAUTH=" --password $2 --username $1"
	fi
	procedure && return 0 || return 1

}
release_me(){
	echo Tar BSPReleasePackage....
	tar cjf BSPReleasePackage-STAR8100.tar.bz2 LinuxBSPReleaseSOP.txt mk_bsp_release.sh $BASE
	echo Done.
}
ret=0
case "$1" in
	create|cr)
		main "$2" "$3"&&ret=0||ret=1
		;;
	-h|"")
		usage;
		;;
	*)
		$1 "$2" "$3" &&ret=0||ret=1;
		;;
esac

exit $ret
