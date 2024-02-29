//included into Decoder.hpp

namespace cbor {
	template<DecoderState State, DecoderState LastState>
	auto Decoder::decode_type_count_length(unsigned char minor_type) -> bool {
		if(minor_type == 24) { // 1 byte
			_current_length = 1;
			_state = State;
		} else if(minor_type == 25) { // 2 byte
			_current_length = 2;
			_state = State;
		} else if(minor_type == 26) { // 4 byte
			_current_length = 4;
			_state = State;
		} else if(minor_type == 27) { // 8 byte
			_current_length = 8;
			_state = LastState;
		} else {
			return false;
		}
		return true;
	}
}
