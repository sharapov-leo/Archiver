#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include "algorithms.h"

void algorithmC(std::vector<unsigned char>& S, std::vector<unsigned char>& L, int& I) {
	int n = S.size();
	L.resize(n);

	std::vector<int> p(n), c(n), cnt(std::max(alphabetSize, n), 0);

	for (int i = 0; i < n; ++i)
		++cnt[S[i]];
	for (int i = 1; i < alphabetSize; ++i)
		cnt[i] += cnt[i - 1];
	for (int i = 0; i < n; ++i)
		p[--cnt[S[i]]] = i;
	c[p[0]] = 0;
	int classes = 1;
	for (int i = 1; i < n; ++i) {
		if (S[p[i]] != S[p[i - 1]])
			++classes;
		c[p[i]] = classes - 1;
	}

	std::vector<int> pn(n), cn(n);
	for (int h = 0; (1 << h) < n; ++h) {
		for (int i = 0; i < n; ++i) {
			pn[i] = p[i] - (1 << h);
			if (pn[i] < 0)
				pn[i] += n;
		}
		std::fill(cnt.begin(), cnt.begin() + classes, 0);
		for (int i = 0; i < n; ++i)
			++cnt[c[pn[i]]];
		for (int i = 1; i < classes; ++i)
			cnt[i] += cnt[i - 1];
		for (int i = n - 1; i >= 0; --i)
			p[--cnt[c[pn[i]]]] = pn[i];
		cn[p[0]] = 0;
		classes = 1;
		for (int i = 1; i < n; ++i) {
			std::pair<int, int> cur = { c[p[i]], c[(p[i] + (1 << h)) % n] };
			std::pair<int, int> prev = { c[p[i - 1]], c[(p[i - 1] + (1 << h)) % n] };
			if (cur != prev)
				++classes;
			cn[p[i]] = classes - 1;
		}
		c.swap(cn);
	}

	for (int i = 0; i < n; ++i) {
		L[i] = (p[i] == 0 ? S[S.size() - 1] : S[p[i] - 1]);
		if (p[i] == 0)
			I = i;
	}
}

void algorithmD(std::vector<unsigned char>& L, int& I, std::vector<unsigned char>& S) {
	int N = L.size();
	S.resize(N);
	std::vector<int> P(N);
	std::vector<int> C(alphabetSize, 0);
	for (int i = 0; i < N; ++i) {
		P[i] = C[L[i]];
		++C[L[i]];
	}

	int sum = 0;
	for (int ch = 0; ch < alphabetSize; ++ch) {
		sum += C[ch];
		C[ch] = sum - C[ch];
	}

	int i = I;
	for (int j = N - 1; j > -1; --j) {
		S[j] = L[i];
		i = P[i] + C[L[i]];
	}
}

void algorithmM1(std::vector<unsigned char>& L, std::list<unsigned char>& X, std::vector<int>& R) {
	int N = L.size();
	std::vector<int> C(alphabetSize);
	for (int i = 0; i < N; ++i)
		++C[L[i]];
	std::list<unsigned char> Y;
	for (int i = alphabetSize - 1; i > -1; --i)
		if (C[i])
			Y.push_front(i);
	X = Y;
	R.resize(N);
	for (int i = 0; i < N; ++i) {
		int pos = 0;
		for (std::list<unsigned char>::iterator j = Y.begin(); j != Y.end(); ++j) {
			if (*j == L[i]) {
				R[i] = pos;
				Y.erase(j);
				Y.push_front(L[i]);
				break;
			}
			++pos;
		}
	}
}

void algorithmW2(std::list<unsigned char>& X, std::vector<int>& R, std::vector<unsigned char>& L) {
	int N = R.size();
	L.resize(N);
	std::list<unsigned char> Y = X;
	std::list<unsigned char>::iterator it;
	for (int i = 0; i < N; ++i) {
		it = Y.begin();
		for (int j = 0; j < R[i]; ++j)
			++it;
		L[i] = *it;
		Y.erase(it);
		Y.push_front(L[i]);
	}
}

void RLEncode(std::vector<int>& R, std::vector<int>& E) {
	int sizeR = R.size();
	int service;

	for (int i = 0; i < sizeR; ++i) {
		int j;

		if ((sizeR - i > 1) && (R[i] == R[i + 1])) {
			j = i + 2;

			while ((j != sizeR) && (j - i < ((UCHARMAX / 2) + 2))) {
				if (R[j] != R[i])
					break;
				++j;
			}

			service = (1 << 7);
			service += (j - i) - 2;
			E.push_back(service);
			E.push_back(R[i]);
			i = j - 1;
			continue;
		}
		j = i + 1;
		service = 0;
		E.push_back(service);
		int posService = E.size() - 1;
		E.push_back(R[i]);
		while ((j != sizeR) && (j - i < ((UCHARMAX / 2) + 1))) {
			if (R[j - 1] == R[j]) {
				E.pop_back();
				--j;
				break;
			}
			E.push_back(R[j]);
			++j;
		}
		E[posService] = (j - i) - 1;
		i = j - 1;
	}

}

void RLDecode(std::vector<int>& E, std::vector<int>& R) {
	int sizeE = E.size();
	int sizeR = 0;
	for (int i = 0; i < sizeE; ++i) {
		int service = E[i];
		if (service & (1 << 7)) {
			service -= (1 << 7);
			service += 2;
		}
		else {
			service += 1;
		}
		sizeR += service;
	}
	R.resize(sizeR);
	R.clear();
	for (int i = 0; i < sizeE; ++i) {
		int service = E[i];
		if (service & (1 << 7)) {
			service -= (1 << 7);
			service += 2;
			for (int j = 0; j < service; ++j)
				R.push_back(E[i + 1]);
			i = (i + 2) - 1;
		}
		else {
			service += 1;
			for (int j = i + 1; j < i + 1 + service; ++j)
				R.push_back(E[j]);
			i = (i + 1 + service) - 1;
		}
	}
}