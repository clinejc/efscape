
CR_HEADER = Copyright.doc
CR_TEMP = ./.temp.cr
CR_SED = ./Sed.cr

copyright : 
	for file in ${FILES};do \
	sed s/__PACKAGE_NAME__/${NAME}/g ${CR_HEADER} | sed s/__FILE_NAME__/$$file/g > ./${CR_TEMP};\
	sed -n -f ${CR_SED} $$file >> ${CR_TEMP};\
	mv ${CR_TEMP} $$file;\
	done
