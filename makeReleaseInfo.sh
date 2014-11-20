#!/bin/bash
#
# makeReleaseInifo.sh
#
#  Created on: Aug 17, 2011
#  Author:     Brian Yi ZHANG
#  Email:      brianlions@gmail.com
#

#set -o xtrace

build_time=`date +'%Y-%m-%d %H:%M:%S %Z'`
build_host=`hostname`
build_user=$USER

include_guard=`date +'_Brian_Yi_ZHANG_RELEASE_H_%Y%m%d_%H%M%S_'`
copyright_info='Copyright (C) 2012 Brian Yi ZHANG (brianlions@gmail.com)'

if [ x"$1" == x ]; then
    release_info_header="release.h"
else
    test -d $1 || mkdir -p $1
    release_info_header="$1/release.h"
fi

# if `svn' is available, and this is a svn repository
which svn 1>/dev/null 2>&1
if [ $? -eq 0 ]; then
    svn info 1>/dev/null 2>&1
    if [ $? -eq 0 ]; then
        svn_commit_id=r`svn info | grep '^Revision:' | cut -d ' ' -f 2`
        svn_dirty=`svn diff --diff-cmd diff 2>/dev/null | wc -l`
    else
        svn_commit_id="unknown"
        svn_dirty="unknown"
    fi
else
    svn_commit_id="unknown"
    svn_dirty="unknown"
fi

# if `git' is available, and this is a git repository
which git 1>/dev/null 2>&1
if [ $? -eq 0 ]; then
    git rev-parse HEAD 1>/dev/null 2>&1
    if [ $? -eq 0 ]; then
        git_commit_id=g`git rev-parse HEAD | cut -b1-7`
        git_dirty=`git diff HEAD 2>/dev/null | wc -l`
        #git_tag=`git tag -l v[0-9]* | tail -1`
        git_tag=`git describe --tags --match v[0-9]* --dirty 2>/dev/null`
        if [ x"$git_tag" == x ]; then
            git_tag="v0.0.0-0-g0000000"
        fi
    else
        git_commit_id="unknown"
        git_dirty="unknown"
        git_tag="unknown"
    fi
else
    git_commit_id="unknown"
    git_dirty="unknown"
    git_tag="unkown"
fi

test -f $release_info_header || touch $release_info_header
# do NOT grep for BUILD_TIME, it always change!
(grep SVN_COMMIT_ID $release_info_header | grep "\"$svn_commit_id\"" >/dev/null) && \
    (grep SVN_DIRTY     $release_info_header | grep "\"$svn_dirty\""     >/dev/null) && \
    (grep GIT_COMMIT_ID $release_info_header | grep "\"$git_commit_id\"" >/dev/null) && \
    (grep GIT_DIRTY     $release_info_header | grep "\"$git_dirty\""     >/dev/null) && \
    (grep GIT_TAG       $release_info_header | grep "\"$git_tag\""       >/dev/null) && \
    (grep BUILD_HOST    $release_info_header | grep "\"$build_host\""    >/dev/null) && \
    (grep BUILD_USER    $release_info_header | grep "\"$build_user\""    >/dev/null) && \
    exit 0

echo "/* AUTO GENERATED, DO NOT EDIT */"                         > $release_info_header
echo "#ifndef $include_guard"                                   >> $release_info_header
echo "#define $include_guard"                                   >> $release_info_header
echo ""                                                         >> $release_info_header
echo "#include <stdio.h>"                                       >> $release_info_header
echo "#include <stdlib.h>"                                      >> $release_info_header
echo ""                                                         >> $release_info_header
echo "#define SVN_COMMIT_ID    \"$svn_commit_id\""              >> $release_info_header
echo "#define SVN_DIRTY        \"$svn_dirty\""                  >> $release_info_header
echo "#define GIT_COMMIT_ID    \"$git_commit_id\""              >> $release_info_header
echo "#define GIT_DIRTY        \"$git_dirty\""                  >> $release_info_header
echo "#define GIT_TAG          \"$git_tag\""                    >> $release_info_header
echo "#define BUILD_TIME       \"$build_time\""                 >> $release_info_header
echo "#define BUILD_HOST       \"$build_host\""                 >> $release_info_header
echo "#define BUILD_USER       \"$build_user\""                 >> $release_info_header
echo "#define BUILD_COPYRIGHT  \"$copyright_info\""             >> $release_info_header
echo ""                                                         >> $release_info_header
echo -e "__inline__ void print_release_info("                   >> $release_info_header
echo -e "        int ec = EXIT_SUCCESS,"                        >> $release_info_header
echo -e "        const char * message = BUILD_COPYRIGHT)"       >> $release_info_header
echo -e "{"                                         	        >> $release_info_header
echo -e "  (void) fprintf(ec == 0 ? stdout : stderr,"		>> $release_info_header
echo -e "         \"Release Info:"'\\n'\"           	        >> $release_info_header
echo -e "         \"  svn commit id: %s"'\\n'\"                 >> $release_info_header
echo -e "         \"  svn dirty:     %s"'\\n'\"                 >> $release_info_header
echo -e "         \"  git tag:       %s"'\\n'\"                 >> $release_info_header
echo -e "         \"  git commit id: %s"'\\n'\"                 >> $release_info_header
echo -e "         \"  git dirty:     %s"'\\n'\"                 >> $release_info_header
echo -e "         \"  build time:    %s"'\\n'\"                 >> $release_info_header
echo -e "         \"  build on host: %s"'\\n'\"                 >> $release_info_header
echo -e "         \"  build by user: %s"'\\n'\"                 >> $release_info_header
echo -e "         \""'\\n'\"                                    >> $release_info_header
echo -e "         \"%s"'\\n'\"                                  >> $release_info_header
echo -e "         , SVN_COMMIT_ID"                              >> $release_info_header
echo -e "         , SVN_DIRTY"                                  >> $release_info_header
echo -e "         , GIT_TAG"                                    >> $release_info_header
echo -e "         , GIT_COMMIT_ID"                              >> $release_info_header
echo -e "         , GIT_DIRTY"                                  >> $release_info_header
echo -e "         , BUILD_TIME"                                 >> $release_info_header
echo -e "         , BUILD_HOST"                                 >> $release_info_header
echo -e "         , BUILD_USER"                                 >> $release_info_header
echo -e "         , message ? message : \"\""                   >> $release_info_header
echo -e "         );"                                           >> $release_info_header
echo -e "  exit(ec);"                                           >> $release_info_header
echo -e "}"                                                     >> $release_info_header
echo "#endif /* $include_guard */"                              >> $release_info_header
echo ""                                                         >> $release_info_header

touch $release_info_header # force recompile

