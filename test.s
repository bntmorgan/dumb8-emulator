/*
Copyright (C) 2012  Carla Sauvanaud
Copyright (C) 2012, 2016 Benoît Morgan

This file is part of dumb8-emulator.

dumb8-emulator is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

dumb8-emulator is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with dumb8-emulator.  If not, see <http://www.gnu.org/licenses/>.
*/

AFC eax #2
PSH eax
AFC eax #3
PSH eax
AFC eax #2
PSH eax
POP ebx
POP eax
MUL eax eax ebx
PSH eax
POP ebx
POP eax
ADD eax eax ebx
PSH eax
POP eax
COP [ebp]-1 eax
PRI [ebp]-1
