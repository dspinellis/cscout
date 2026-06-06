/*
 * (C) Copyright 2008-2026 Diomidis Spinellis
 *
 * This file is part of CScout.
 *
 * CScout is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CScout is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CScout.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Display loop progress indicator.
 *
 */

 #ifndef PROGRESS_H
 #define PROGRESS_H

 #include <iostream>


 // display loop progress (non-reentrant)
 template <typename container>
 static void
 progress(typename container::const_iterator i, const container &c)
 {
    static int count, opercent;

    if (i == c.begin())
        count = 0;
    int percent = ++count * 100 / c.size();
    if (percent != opercent) {
            std::cerr << '\r' << percent << '%' << std::flush;
            opercent = percent;
        }
 }

 #endif /* PROGRESS_H */