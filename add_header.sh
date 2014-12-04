#!/bin/bash
author_name="Brian Yi ZHANG"
author_email="brianlions@gmail.com"

for fn in "$@"
do
    # get first commit id of the specified file
    commit_id=`git log --oneline "$fn" | tail -1 | gawk '{ print $1 }'`
    # get date and time from commit message of that commit
    long_date=`git log $commit_id -1 | grep Date | cut -c 9-32`
    # format the date and time
    short_date=`date -d "$long_date" +"%Y/%m/%d %H:%M:%S"`

    # add header to the file
    sed -i -e '1 i\
/*\
 * file name:           '"$fn"'\
 *\
 * author:              '"$author_name"'\
 * email:               '"$author_email"'\
 * date created:        '"$short_date"'\
 */' "$fn"
done
