// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"
#define bpe 32

// Fake variables used as placeholders in tests
static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);

TBitField::TBitField(int len)
{
    if (len < 0)
        throw std::invalid_argument("TBitField: negative length");

    BitLen = len;
    MemLen = (len + bpe - 1) / bpe;
    if (MemLen == 0) MemLen = 1;

    pMem = new TELEM[MemLen];
    std::memset(pMem, 0, MemLen * sizeof(TELEM));
}

TBitField::TBitField(const TBitField &bf) // конструктор копирования
{
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    pMem = new TELEM[MemLen];
    for (int i = 0;i < MemLen;i++) {
        pMem[i] = bf.pMem[i];
    }
}
TBitField::TBitField(TBitField&& bf) noexcept // конструктор копирования
{
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    pMem = bf.pMem;

    bf.pMem = nullptr;
    bf.BitLen = 0;
    bf.MemLen = 0;
}

TBitField::~TBitField()
{
    delete[] pMem;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
    return n/bpe;
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{
    return static_cast<TELEM>(1u) << (n % bpe);
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
  return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
    if (n < 0 || n >= BitLen)
        throw std::out_of_range("SetBit: index out of range");

    pMem[GetMemIndex(n)] |= GetMemMask(n);
}

void TBitField::ClrBit(const int n) // очистить бит
{
    if (n < 0 || n >= BitLen)
        throw std::out_of_range("ClrBit: index out of range");

    pMem[GetMemIndex(n)] &= ~GetMemMask(n);
}

int TBitField::GetBit(const int n) const // получить значение бита
{
    if (n < 0 || n >= BitLen)
        throw std::out_of_range("GetBit: index out of range");

    return (pMem[GetMemIndex(n)] & GetMemMask(n)) ? 1 : 0;
}

// битовые операции

TBitField& TBitField::operator=(const TBitField &bf) // присваивание
{
    if (this == &bf) return *this;
    delete[] pMem;
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    pMem = new TELEM[MemLen];
    std::memcpy(pMem, bf.pMem, MemLen * sizeof(TELEM));
    return *this;
}

TBitField& TBitField::operator=(TBitField&& bf) // присваивание
{
    if (this == &bf) return *this;

    delete[] pMem;

    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    pMem = bf.pMem;

    bf.BitLen = 0;
    bf.MemLen = 0;
    bf.pMem = nullptr;

    return *this;
}

int TBitField::operator==(const TBitField &bf) const // сравнение
{
    int res = 0;

    if (BitLen != bf.BitLen) {
        res = 0;
    }
    else {
        for (int i = 0; i < MemLen; i++) {
            TELEM memMask = 0;
            if (i < MemLen - 1) {
                memMask = 0xffffffff;
            }
            else {
                memMask = (1 << (BitLen % 32)) - 1;
            }

            if ((memMask & pMem[i]) != (memMask & bf.pMem[i])) {
                res = 0;
                break;
            }
        }
    }

    return res;
}

int TBitField::operator!=(const TBitField &bf) const // сравнение
{
    return !(*this == bf);
}

TBitField TBitField::operator|(const TBitField &bf) // операция "или"
{
    if (this->BitLen != bf.BitLen) {
        this->BitLen = bf.BitLen;
    }

    TBitField tmp(BitLen);

    for (int i = 0; i < MemLen; i++) {
        tmp.pMem[i] = pMem[i];
    }

    for (int j = 0; j < MemLen; j++) {
        tmp.pMem[j] |= bf.pMem[j];
    }

    return tmp;
}

TBitField TBitField::operator&(const TBitField &bf) // операция "и"
{
    if (this->BitLen != bf.BitLen) {
        this->BitLen = bf.BitLen;
    }

    TBitField tmp(BitLen);

    for (int i = 0; i < MemLen; i++) {
        tmp.pMem[i] = pMem[i];
    }

    for (int j = 0; j < MemLen; j++) {
        tmp.pMem[j] &= bf.pMem[j];
    }

    return tmp;
}

TBitField TBitField::operator~(void) // отрицание
{
    TBitField tmp(BitLen);

    for (int i = 0; i < MemLen; i++) {
        tmp.pMem[i] = ~pMem[i];
    }

    return tmp;
}

// ввод/вывод

istream &operator>>(istream &istr, TBitField &bf) // ввод
{
    int i = 0;
    char s;

    while (1) {
        istr >> s;
        if (s == '1') {
            bf.SetBit(i++);
        }
        else {
            if (s == '0') {
                bf.ClrBit(i++);
            }
            else {
                break;
            }
        }
    }

    return istr;
}

ostream &operator<<(ostream &ostr, const TBitField &bf) // вывод
{
    int len = bf.BitLen;

    for (int i = 0; i < len; i++) {
        if (bf.GetBit(i)) {
            ostr << "1";
        }
        else {
            ostr << "0";
        }
    }

    return ostr;
}
