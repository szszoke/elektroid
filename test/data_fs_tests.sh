#!/usr/bin/env bash

function get_sound_n_with_id () {
  s="sound$1"
  echo "${!s}" | sed "s/^F   $1 0012/F  $2 007e/"
}

echo "Getting devices..."
DEVICE=$($ecli ld | head -n 1 | awk '{print $1}')
[ -z "$DEVICE" ] && echo "No device found" && exit 0
echo "Using device $DEVICE..."

sound1=$($ecli ls-data $DEVICE:/soundbanks/A | grep "^F   1")
nsound1=$(get_sound_n_with_id 1 64)

sound2=$($ecli ls-data $DEVICE:/soundbanks/A | grep "^F   2")

echo "Testing data copy..."
$ecli cp-data $DEVICE:/soundbanks/A/1 $DEVICE:/soundbanks/H/64
[ $? -ne 0 ] && exit 1
$ecli cp-data $DEVICE:/soundbanks/A/2 $DEVICE:/soundbanks/H/63
[ $? -ne 0 ] && exit 1
output=$($ecli ls-data $DEVICE:/soundbanks/H)
actual=$(echo "$output" | grep "^F  64")
expected=$(get_sound_n_with_id 1 64)
[ "$actual" != "$expected" ] && exit 1
actual=$(echo "$output" | grep "^F  63")
expected=$(get_sound_n_with_id 2 63)
[ "$actual" != "$expected" ] && exit 1

echo "Testing data move..."
$ecli mv-data $DEVICE:/soundbanks/H/64 $DEVICE:/soundbanks/H/62
[ $? -ne 0 ] && exit 1
output=$($ecli ls-data $DEVICE:/soundbanks/H)
actual=$(echo "$output" | grep "^F  62")
expected=$(get_sound_n_with_id 1 62)
[ "$actual" != "$expected" ] && exit 1
actual=$(echo "$output" | grep "^F  64")
[ -n "$actual" ] && exit 1

echo "Testing data swap..."
$ecli sw-data $DEVICE:/soundbanks/H/62 $DEVICE:/soundbanks/H/63
[ $? -ne 0 ] && exit 1
output=$($ecli ls-data $DEVICE:/soundbanks/H)
actual=$(echo "$output" | grep "^F  62")
expected=$(get_sound_n_with_id 2 62)
[ "$actual" != "$expected" ] && exit 1
actual=$(echo "$output" | grep "^F  63")
expected=$(get_sound_n_with_id 1 63)
[ "$actual" != "$expected" ] && exit 1

echo "Testing data clear..."
$ecli cl-data $DEVICE:/soundbanks/H/63
[ $? -ne 0 ] && exit 1
$ecli cl-data $DEVICE:/soundbanks/H/62
[ $? -ne 0 ] && exit 1
output=$($ecli ls-data $DEVICE:/soundbanks/H)
[ $(echo "$output" | grep "^F  62" | wc -l) -ne 0 ] && exit 1
[ $(echo "$output" | grep "^F  63" | wc -l) -ne 0 ] && exit 1

echo "Testing upload..."
$ecli ul-data $srcdir/res/SOUND.dtdata $DEVICE:/soundbanks/H
[ $? -ne 0 ] && exit 1
id=$($ecli ls-data $DEVICE:/soundbanks/H | grep 'SOUND$' | awk '{print $2}')

echo "Testing download..."
$ecli dl-data $DEVICE:/soundbanks/H/$id
[ $? -ne 0 ] && exit 1
ls "SOUND.dtdata"
cksum SOUND.dtdata
cksum $srcdir/res/SOUND.dtdata
actual_cksum="$(cksum SOUND.dtdata | awk '{print $1}')"
[ "$actual_cksum" != "$(cksum $srcdir/res/SOUND.dtdata | awk '{print $1}')" ] && exit 1
rm SOUND.dtdata
[ $? -ne 0 ] && exit 1
$ecli cl-data $DEVICE:/soundbanks/H/$id
[ $? -ne 0 ] && exit 1

exit 0
