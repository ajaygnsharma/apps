#!/bin/bash
set -x
rsync -avz -e "ssh -p 20015" --delete -r /home/user1/Documents/omv_my_doc/ omv@agns.no-ip.org:/export/doc/books_articles/my/
