#!/bin/bash

git-archive-all source-vendored.tar.gz

tar xzf source-vendored.tar.gz
rm -rf source-vendored.tar.gz

cd source-vendored/src/engine/riti
cargo vendor vendor

mkdir .cargo && cd .cargo

cat > config.toml << EOF
[source.crates-io]
replace-with = "vendored-sources"

[source.vendored-sources]
directory = "vendor"
EOF

cd ../../../../../

tar -czf source-vendored.tar.gz source-vendored

rm -rf source-vendored
