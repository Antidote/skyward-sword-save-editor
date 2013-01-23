REV=`svnversion -n ../ | sed -e 's/.*://' -e 's/\([0-9]*\).*/\1/' | grep '[0-9]'`
echo $REV
cat > ./svnrev.h << EOF
#define SVN_REV $REV
#define SVN_REV_STR "$REV"
EOF
