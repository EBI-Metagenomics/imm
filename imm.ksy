meta:
  id: imm
  file-extension: imm
  endian: le
seq:
  - id: hmm
    type: hmm
  - id: dp
    type: dp
enums:
  state_type:
    0: mute
    1: normal
    2: table
types:
  hmm:
    seq:
      - id: abc
        type: abc
      - id: nstates
        type: u4
      - id: states
        type: state
        repeat: expr
        repeat-expr: nstates
      - id: ntransitions
        type: u4
      - id: transitions
        type: transition
        repeat: expr
        repeat-expr: ntransitions
  abc:
    seq:
      - id: nsymbols
        type: u2
      - id: symbols
        type: str
        encoding: ASCII
        size: nsymbols + 1
        terminator: 0
      - id: any_symbol
        type: str
        encoding: ASCII
        size: 1
  state:
    seq:
      - id: name_length
        type: u2
      - id: name
        type: str
        encoding: ASCII
        size: name_length + 1
        terminator: 0
      - id: start_lprob
        type: f8
      - id: state_type
        type: u1
        enum: state_type
      - id: impl_chunk_size
        type: u4
      - id: mute_state
        type: mute_state
        if: state_type == state_type::mute
      - id: normal_state
        type: normal_state
        if: state_type == state_type::normal
  mute_state: {}
  normal_state:
    seq:
      - id: lprobs_size
        type: u1
      - id: lprobs
        type: f8
        repeat: expr
        repeat-expr: lprobs_size
  table_state: {}
  transition:
    seq:
      - id: source_state
        type: u4
      - id: target_state
        type: u4
      - id: lprob
        type: f8
  dp:
    seq:
      - id: seq_code
        type: seq_code
      - id: dp_emission
        type: dp_emission
      - id: dp_trans_table
        type: dp_trans_table
      - id: dp_state_table
        type: dp_state_table
  seq_code:
    seq:
      - id: min_seq
        type: u1
      - id: max_seq
        type: u1
      - id: offset
        type: u4
        repeat: expr
        repeat-expr: max_seq - min_seq + 1
      - id: stride
        type: u4
        repeat: expr
        repeat-expr: max_seq
      - id: size
        type: u4
  dp_emission:
    seq:
      - id: score_size
        type: u4
      - id: score
        type: f8
        repeat: expr
        repeat-expr: score_size
      - id: offset_size
        type: u4
      - id: offset
        type: u4
        repeat: expr
        repeat-expr: offset_size
  dp_trans_table:
    seq:
      - id: ntrans
        type: u4
      - id: score
        type: f8
        repeat: expr
        repeat-expr: ntrans
      - id: source_state
        type: u4
        repeat: expr
        repeat-expr: ntrans
      - id: offset_size
        type: u4
      - id: offset
        type: u4
        repeat: expr
        repeat-expr: offset_size
  dp_state_table:
    seq:
      - id: nstates
        type: u4
      - id: min_seq
        type: u1
        repeat: expr
        repeat-expr: nstates
      - id: max_seq
        type: u1
        repeat: expr
        repeat-expr: nstates
      - id: start_lprob
        type: f8
        repeat: expr
        repeat-expr: nstates
      - id: end_state
        type: u4
