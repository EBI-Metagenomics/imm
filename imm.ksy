meta:
  id: imm
  file-extension: imm
  endian: le
seq:
  - id: symbols_size
    type: u2
  - id: symbols
    type: str
    encoding: ASCII
    size: symbols_size
    terminator: 0
  - id: any_symbol
    type: str
    encoding: ASCII
    size: 1
  - id: nstates
    type: u4
  - id: states
    type: state
    repeat: expr
    repeat-expr: nstates
  - id: seq_code
    type: seq_code
types:
  state:
    seq:
      - id: state_type
        type: u1
      - id: name_size
        type: u2
      - id: name
        type: str
        encoding: ASCII
        size: name_size
        terminator: 0
      - id: start_lprob
        type: f8
      - id: impl_chunk_size
        type: u4
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
