#include "bitarray.h"
#include <string.h>


BitArray::BitArray(uint8_t *array, uint32_t slots, uint8_t bitsPerSlot) :
		Array(array), Slots(slots), BitsPerSlot(bitsPerSlot), NumBytes(((Slots * BitsPerSlot) / 8) + 1) {
	clear();
}

void BitArray::clear() {
	memset(&Array[0], 0, NumBytes);
}

uint8_t BitArray::getValueAsByte(uint32_t slot) {
	uint32_t byte = (slot * BitsPerSlot) / 8;
	uint32_t bitOffSet = (slot * BitsPerSlot) % 8;
	uint32_t retVal = 0;
	for (int i = 0; i < BitsPerSlot; ++i) {
		if (Array[byte] & (1 << bitOffSet)) {
			retVal |= (1 << i);
		}
		bitOffSet = (++bitOffSet) % 8;
		if (!bitOffSet)
			byte++;
	}
	return retVal & 0xFF;
}

void BitArray::setValueAsByte(uint32_t slot, uint8_t value) {
	uint32_t byte = (slot * BitsPerSlot) / 8;
	uint32_t bitOffSet = (slot * BitsPerSlot) % 8;
	for (int i = 0; i < BitsPerSlot; ++i) {
		if (value & (1 << i)) {
			Array[byte] |= (1 << bitOffSet);
		} else {
			Array[byte] &= ~(1 << bitOffSet);
		}
		bitOffSet = ++bitOffSet % 8;
		if (!bitOffSet)
			byte++;
	}
}
