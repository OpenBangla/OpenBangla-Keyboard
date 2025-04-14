#!/bin/bash
echo "Adding Fedora tweaks..."
echo "max_parallel_downloads=19" | sudo tee -a /etc/dnf/dnf.conf