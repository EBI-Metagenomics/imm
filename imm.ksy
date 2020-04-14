meta:
  id: imm
  file-extension: imm
  endian: le
seq:
  - id: hmm
    type: hmm
#  - id: seq_code
#    type: seq_code
#  - id: dp_emission
#    type: dp_emission
types:
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
      - id: impl_chunk_size
        type: u4
      - id: impl_chunk
        size: impl_chunk_size
  transition:
    seq:
      - id: source_state
        type: u4
      - id: target_state
        type: u4
      - id: lprob
        type: f8
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
