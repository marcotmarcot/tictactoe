#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>

enum Cell {
  EMPTY = 0,
  SELF = 1,
  ADVERSARY = 2,
};

enum Status {
  PLAYING = 0,
  VICTORY = 1,
  DEFEAT = 2,
  DRAW = 3,
};

bool BetterThan(const Status victory, const Status defeat, const Status a, const Status b) {
  if (a == victory) {
    return false;
  }
  if (a == defeat) {
    return b != defeat;
  }
  return b == victory;
}
    
bool BetterThan(const bool adversary, const Status a, const Status b) {
  if (adversary) {
    return BetterThan(DEFEAT, VICTORY, a, b);
  }
  return BetterThan(VICTORY, DEFEAT, a, b);
}

class Board {
public:
  std::optional<Board> Play(const int pos, const bool adversary) const {
    if (Get(pos) != EMPTY) {
      return {};
    }
    auto set = Set(pos, adversary);
    Print();
    std::cout << *this << ".Set(" << pos << ", " << adversary << ")" << std::endl;
    set.Print();
    std::cout << std::endl;
    return set.Normalize();
  }

  Status GetStatus() const {
    bool full = true;
    for (int i = 0; i < 3; ++i) {
      if (Line(i, full)) {
	return Status(Get(i, 0));
      }
    }
    for (int j = 0; j < 3; ++j) {
      if (Column(j, full)) {
	return Status(Get(0, j));
      }
    }
    if (Diagonal1(full)) {
      return Status(Get(0, 0));
    }
    if (Diagonal2(full)) {
      return Status(Get(0, 2));
    }
    if (full) {
      return DRAW;
    }
    return PLAYING;
  }

  void Print() const {
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
	std::cout << Get(i, j);
      }
      std::cout << std::endl;
    }
  }

  bool operator==(const Board& other) const = default;

  friend std::ostream& operator<<(std::ostream& os, const Board board) {
    os << board.board_;
    return os;
  }

private:
  int board_ = 0;

  Cell Get(const int i, const int j) const {
    return Get(i * 3 + j);
  }

  Cell Get(const int pos) const {
    return Cell((board_ >> (pos * 2)) % 4);
  }

  Board Set(const int pos, const bool adversary) const {
    std::cout << "adversary=" << adversary << ", cell=" << Cell(int(adversary) + 1) << std::endl;
    return Set(pos, Cell(int(adversary) + 1));
  }
  
  Board Set(const int pos, const Cell cell) const {
    std::cout << *this << ".Set(" << pos << ", " << cell << ")" << std::endl;
    Board next(*this);
    std::cout << next << std::endl;
    next.board_ &= ~(3 << (pos * 2));
    std::cout << next << std::endl;
    next.board_ |= cell << (pos * 2);
    std::cout << next << std::endl;
    std::cout << std::endl;
    return next;
  }

  Board Normalize() const {
    return NormalizeWithRotations(*this).NormalizeWithRotations(Mirror());
  }

  Board NormalizeWithRotations(const Board reference) const {
    Print();
    std::cout << "NormalizeWithRotations()" << std::endl;
    Board normalized(*this);
    for (int i = 0; i < 4; ++i) {
      normalized.board_ = std::min(normalized.board_, reference.Rotate(i).board_);
    }
    normalized.Print();
    std::cout << std::endl;
    return normalized;
  }

  Board Mirror() const {
    Board mirror(*this);
    for (int i = 0; i < 9; i += 3) {
      mirror.Swap(*this, i, i + 2);
    }
    return mirror;
  }

  Board Rotate(const int count) const {
    if (count == 0) {
      return *this;
    }
    Board rotated(*this);
    switch (count) {
    case 1: {
      rotated.Swap(*this, 0, 2, 8, 6);
      rotated.Swap(*this, 1, 5, 7, 3);
      break;
    }
    case 2: {
      for (int i = 0; i < 4; ++i) {
	rotated.Swap(*this, i, 8 - i);
      }
      break;
    }
    case 3: {
      rotated.Swap(*this, 0, 6, 8, 2);
      rotated.Swap(*this, 1, 3, 7, 5);
      break;
    }
    }
    return rotated;
  }

  void Swap(const Board& reference, const int i1, const int i2) {
    Set(i2, reference.Get(i1));
    Set(i1, reference.Get(i2));
  }

  void Swap(const Board& reference, const int i1, const int i2, const int i3, const int i4)  {
    Set(i2, reference.Get(i1));
    Set(i3, reference.Get(i2));
    Set(i4, reference.Get(i3));
    Set(i1, reference.Get(i4));
  }

  bool Line(const int i, bool& full) const {
    const Cell first = Get(i, 0);
    if (first == EMPTY) {
      full = false;
      return false;
    }
    for (int j = 1; j < 3; ++j) {
      const Cell current = Get(i, j);
      if (current == EMPTY) {
	full = false;
	return false;
      }
      if (first != current) {
	return false;
      }
    }
    return true;
  }

  bool Column(const int j, bool& full) const {
    const Cell first = Get(0, j);
    if (first == EMPTY) {
      full = false;
      return false;
    }
    for (int i = 1; i < 3; ++i) {
      const Cell current = Get(i, j);
      if (current == EMPTY) {
	full = false;
	return false;
      }
      if (first != current) {
	return false;
      }
    }
    return true;
  }

  bool Diagonal1(bool& full) const {
    const Cell first = Get(0, 0);
    if (first == EMPTY) {
      full = false;
      return false;
    }
    for (int i = 1; i < 3; ++i) {
      const Cell current = Get(i, i);
      if (current == EMPTY) {
	full = false;
	return false;
      }
      if (first != current) {
	return false;
      }
    }
    return true;
  }

  bool Diagonal2(bool& full) const {
    const Cell first = Get(0, 2);
    if (first == EMPTY) {
      full = false;
      return false;
    }
    for (int i = 1; i < 3; ++i) {
      const Cell current = Get(i, 2 - i);
      if (current == EMPTY) {
	full = false;
	return false;
      }
      if (first != current) {
	return false;
      }
    }
    return true;
  }

  template <typename T> friend class std::hash;
};

template<>
struct std::hash<Board> {
  std::size_t operator()(const Board board) const noexcept {
    return std::hash<int>{}(board.board_);
  }
};

class PlayWriter {
public:
  PlayWriter(const std::string& path) {
    file.open(path);
  }
  
  void Write() {
    Board board;
    Minimax(board, false);
    Minimax(board, true);
  }

private:
  std::ofstream file;
  std::unordered_map<Board, Status> cache;
  
  Status Minimax(const Board board, const bool adversary) {
    Status best = adversary ? VICTORY : DEFEAT;
    std::set<int> victories;
    std::set<int> draws;
    for (int i = 0; i < 9; ++i) {
      const auto next = board.Play(i, adversary);
      if (!next) {
	continue;
      }
      board.Print();
      std::cout << board << ".Play(" << i << ") = " << *next << std::endl;
      next->Print();
      std::cout << std::endl;
      Status status;
      const auto cached = cache.find(*next);
      if (cached != cache.end()) {
	status = cached->second;
      } else {
	status = board.GetStatus();
	if (status == PLAYING) {
	  status = Minimax(*next, !adversary);
	}
	cache[*next] = status;
      }
      if (BetterThan(adversary, status, best)) {
	best = status;
      }
      if (adversary) {
	continue;
      }
      if (status == VICTORY) {
	victories.insert(i);
      }
      if (best != VICTORY && status == DRAW) {
	draws.insert(i);
      }
    }
    if (!adversary) {
      if (!victories.empty()) {
	file << board << ", victory:";
	for (const auto victory : victories) {
	  file << " " << victory;
	}
	file << std::endl;
      } else if (!draws.empty()) {
	file << board << ", draw:";
	for (const auto draw : draws) {
	  file << " " << draw;
	}
	file << std::endl;
      }
    }
    return best;
  }
};


int main() {
  PlayWriter writer("plays.txt");
  writer.Write();
  return 0;
}
