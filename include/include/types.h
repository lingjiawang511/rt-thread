#pragma once

#define CLRSCREEN       "\033[H\033[J" //escape + '[ '+  '2' + 'J'

#define MKFOURCC(a, b, c, d)    (((a) << 24) | (b) << 16 | ((c) << 8) | (d))
