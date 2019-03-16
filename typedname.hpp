class TypedName : public Letter {
public:
  void Append(int ch) {
    txt.append(ch, 1);
    Refresh();
  }
  void BackSpace() {
    txt.erase(txt.end());
    gamekernel::drawObj(x + txt.length(),y, ' ');
    Refresh();
  }
  void ClearAll() {
    gamekernel::clearObjDisp(x,y,txt);
    txt.erase(txt.begin(), txt.end());
  }
}
