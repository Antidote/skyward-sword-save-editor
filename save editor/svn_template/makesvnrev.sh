REV=`svnversion -n ../`
echo $REV
cat > ../SS_SaveEditor/svnrev.h <<EOF
#define SVN_REV $REV
#define SVN_REV_STR "$REV"
EOF
