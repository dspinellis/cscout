#
# Build CScout and its prerequisites
#
# (C) Copyright 2016 Diomidis Spinellis
#
# This file is part of CScout.
#
# CScout is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# CScout is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with CScout.  If not, see <http://www.gnu.org/licenses/>.
#

.PHONY: src/build/cscout swill/libswill.a btyacc/btyacc

src/build/cscout: swill/libswill.a btyacc/btyacc
	cd src && $(MAKE)

swill/libswill.a: swill
	cd swill && $(MAKE)

swill:
	git clone https://github.com/dspinellis/swill.git
	cd swill && ./configure

btyacc/btyacc: btyacc
	cd btyacc && $(MAKE)

btyacc:
	git clone https://github.com/dspinellis/btyacc
