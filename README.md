# Computer Networks – Machine Problems

Introduction to Computer Networks 수업에서 진행한 2개의 머신 과제를 정리했습니다.  
CRC 기반 오류 검출과 라우팅 알고리즘(Distance Vector, Link State)을 구현하며 네트워크 동작을 학습했습니다.

---

## 목차

- [기술 스택](#-기술-스택)
- [MP1: Cyclic Redundancy Check](#-mp1-cyclic-redundancy-check)
- [MP2: Routing Protocols](#-mp2-routing-protocols)
- [실행 예시](#-실행-예시)

---

## 🛠 기술 스택
Language : C / C++
Compiler : g++
Environment : Linux (cspro)


---

## 🧪 MP1: Cyclic Redundancy Check

### 목표
- CRC를 이용해 **전송 데이터의 오류를 검출**한다.

### 구현
- `crc_encoder`: input 파일을 dataword(4/8bit)로 나눠 codeword로 변환(CRC remainder 부착)
- `crc_decoder`: codeword를 dataword로 복원하며 오류 검출, `result_file`에 통계 출력

### 핵심 포인트
- generator로 **modulo-2 division** 수행
- codeword 비트열은 8의 배수가 아닐 수 있어 **앞쪽 zero-padding**
- output 첫 1바이트에 **padding 비트 수 기록**
- decoder는 전체 codeword 수와 오류 codeword 수를 기록

---

## 🧪 MP2: Routing Protocols

### 목표
- **Distance Vector / Link State** 알고리즘으로 라우팅 테이블을 생성하고,
`changes`에 따라 토폴로지 변경 시 테이블을 갱신한다.

### 구현
- `distvec`  → 출력: `output_dv.txt`
- `linkstate` → 출력: `output_ls.txt`

### 입력 파일
- `topologyfile`: 노드 수 + 링크(u v cost)
- `messagesfile`: `src dst message...`
- `changesfile`: 링크 변경(u v cost), `-999`는 링크 제거

### 출력 규칙(요약)
- 초기(변경 적용 전) 라우팅 테이블 + 메시지 출력
- 이후 changes 한 줄 적용할 때마다:
- 라우팅 테이블 출력
- 메시지 전달 시뮬레이션 출력

### 메시지 포맷
- 경로 있음: from x to y cost C hops ... message ...
- 경로 없음: from x to y cost infinite hops unreachable message ...


---

## 🚀 실행 예시

### MP1
```bash
./crc_encoder datastream.tx codedstream.tx 1101 4
./linksim codedstream.tx codedstream.rx 0.05 1001
./crc_decoder codedstream.rx datastream.rx result.txt 1101 4
```
### MP2
```bash
./linkstate topology.txt messages.txt changes.txt
./distvec   topology.txt messages.txt changes.txt
```
---
