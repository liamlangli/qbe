#include "all.h"
#include <ctype.h>
#include <stdarg.h>

enum {
	Ke = -2, /* Erroneous mode */
	Kx = -1, /* Invalid operand */
	Km = Kl, /* Memory pointer (for x64) */
};

OpDesc opdesc[NOp] = {
#define A(a,b,c,d) {[Kw]=K##a, [Kl]=K##b, [Ks]=K##c, [Kd]=K##d}

	/*            NAME       NM      ARGCLS0     ARGCLS1  SF LF FLD*/
	[OAdd]    = { "add",      2, {A(w,l,s,d), A(w,l,s,d)}, 1, 0, 1 },
	[OSub]    = { "sub",      2, {A(w,l,s,d), A(w,l,s,d)}, 1, 0, 1 },
	[ODiv]    = { "div",      2, {A(w,l,s,d), A(w,l,s,d)}, 0, 0, 1 },
	[ORem]    = { "rem",      2, {A(w,l,e,e), A(w,l,e,e)}, 0, 0, 1 },
	[OUDiv]   = { "udiv",     2, {A(w,l,e,e), A(w,l,e,e)}, 0, 0, 1 },
	[OURem]   = { "urem",     2, {A(w,l,e,e), A(w,l,e,e)}, 0, 0, 1 },
	[OMul]    = { "mul",      2, {A(w,l,s,d), A(w,l,s,d)}, 0, 0, 1 },
	[OAnd]    = { "and",      2, {A(w,l,e,e), A(w,l,e,e)}, 1, 0, 1 },
	[OOr]     = { "or",       2, {A(w,l,e,e), A(w,l,e,e)}, 1, 0, 1 },
	[OXor]    = { "xor",      2, {A(w,l,e,e), A(w,l,e,e)}, 1, 0, 1 },
	[OSar]    = { "sar",      1, {A(w,l,e,e), A(w,w,e,e)}, 1, 0, 1 },
	[OShr]    = { "shr",      1, {A(w,l,e,e), A(w,w,e,e)}, 1, 0, 1 },
	[OShl]    = { "shl",      1, {A(w,l,e,e), A(w,w,e,e)}, 1, 0, 1 },
	[OStored] = { "stored",   0, {A(d,d,d,d), A(m,m,m,m)}, 0, 1, 0 },
	[OStores] = { "stores",   0, {A(s,s,s,s), A(m,m,m,m)}, 0, 1, 0 },
	[OStorel] = { "storel",   0, {A(l,l,l,l), A(m,m,m,m)}, 0, 1, 0 },
	[OStorew] = { "storew",   0, {A(w,w,w,w), A(m,m,m,m)}, 0, 1, 0 },
	[OStoreh] = { "storeh",   0, {A(w,w,w,w), A(m,m,m,m)}, 0, 1, 0 },
	[OStoreb] = { "storeb",   0, {A(w,w,w,w), A(m,m,m,m)}, 0, 1, 0 },
	[OLoad]   = { "load",     0, {A(m,m,m,m), A(x,x,x,x)}, 0, 1, 0 },
	[OLoadsw] = { "loadsw",   0, {A(m,m,e,e), A(x,x,e,e)}, 0, 1, 0 },
	[OLoaduw] = { "loaduw",   0, {A(m,m,e,e), A(x,x,e,e)}, 0, 1, 0 },
	[OLoadsh] = { "loadsh",   0, {A(m,m,e,e), A(x,x,e,e)}, 0, 1, 0 },
	[OLoaduh] = { "loaduh",   0, {A(m,m,e,e), A(x,x,e,e)}, 0, 1, 0 },
	[OLoadsb] = { "loadsb",   0, {A(m,m,e,e), A(x,x,e,e)}, 0, 1, 0 },
	[OLoadub] = { "loadub",   0, {A(m,m,e,e), A(x,x,e,e)}, 0, 1, 0 },
	[OExtsw]  = { "extsw",    0, {A(e,w,e,e), A(e,x,e,e)}, 0, 1, 1 },
	[OExtuw]  = { "extuw",    0, {A(e,w,e,e), A(e,x,e,e)}, 0, 1, 1 },
	[OExtsh]  = { "extsh",    0, {A(w,w,e,e), A(x,x,e,e)}, 0, 1, 1 },
	[OExtuh]  = { "extuh",    0, {A(w,w,e,e), A(x,x,e,e)}, 0, 1, 1 },
	[OExtsb]  = { "extsb",    0, {A(w,w,e,e), A(x,x,e,e)}, 0, 1, 1 },
	[OExtub]  = { "extub",    0, {A(w,w,e,e), A(x,x,e,e)}, 0, 1, 1 },
	[OExts]   = { "exts",     0, {A(e,e,e,s), A(e,e,e,x)}, 0, 1, 1 },
	[OTruncd] = { "truncd",   0, {A(e,e,d,e), A(e,e,x,e)}, 0, 1, 1 },
	[OFtosi]  = { "ftosi",    0, {A(s,d,e,e), A(x,x,e,e)}, 0, 1, 1 },
	[OSitof]  = { "sitof",    0, {A(e,e,w,l), A(e,e,x,x)}, 0, 1, 1 },
	[OCast]   = { "cast",     0, {A(s,d,w,l), A(x,x,x,x)}, 0, 1, 1 },
	[OCopy]   = { "copy",     1, {A(w,l,s,d), A(x,x,x,x)}, 0, 1, 0 },
	[ONop]    = { "nop",      0, {A(x,x,x,x), A(x,x,x,x)}, 0, 1, 0 },
	[OSwap]   = { "swap",     2, {A(w,l,s,d), A(w,l,s,d)}, 0, 0, 0 },
	[OSign]   = { "sign",     0, {A(w,l,e,e), A(x,x,e,e)}, 0, 0, 0 },
	[OSAlloc] = { "salloc",   0, {A(e,l,e,e), A(e,x,e,e)}, 0, 0, 0 },
	[OXDiv]   = { "xdiv",     1, {A(w,l,e,e), A(x,x,e,e)}, 0, 0, 0 },
	[OXCmp]   = { "xcmp",     1, {A(w,l,s,d), A(w,l,s,d)}, 1, 0, 0 },
	[OXTest]  = { "xtest",    1, {A(w,l,e,e), A(w,l,e,e)}, 1, 0, 0 },
	[OAddr]   = { "addr",     0, {A(m,m,e,e), A(x,x,e,e)}, 0, 1, 0 },
	[OPar]    = { "parn",     0, {A(x,x,x,x), A(x,x,x,x)}, 0, 0, 0 },
	[OParc]   = { "parc",     0, {A(e,x,e,e), A(e,x,e,e)}, 0, 0, 0 },
	[OArg]    = { "arg",      0, {A(w,l,s,d), A(x,x,x,x)}, 0, 0, 0 },
	[OArgc]   = { "argc",     0, {A(e,x,e,e), A(e,l,e,e)}, 0, 0, 0 },
	[OCall]   = { "call",     0, {A(m,m,m,m), A(x,x,x,x)}, 0, 0, 0 },
	[OXSetnp] = { "xsetnp",   0, {A(x,x,e,e), A(x,x,e,e)}, 0, 0, 0 },
	[OXSetp]  = { "xsetp",    0, {A(x,x,e,e), A(x,x,e,e)}, 0, 0, 0 },
	[OAlloc]   = { "alloc4",  1, {A(e,l,e,e), A(e,x,e,e)}, 0, 0, 0 },
	[OAlloc+1] = { "alloc8",  1, {A(e,l,e,e), A(e,x,e,e)}, 0, 0, 0 },
	[OAlloc+2] = { "alloc16", 1, {A(e,l,e,e), A(e,x,e,e)}, 0, 0, 0 },
#define X(c) \
	[OCmpw+IC##c] = { "c"    #c "w", 0, {A(w,w,e,e), A(w,w,e,e)}, 1, 0, 1 }, \
	[OCmpl+IC##c] = { "c"    #c "l", 0, {A(l,l,e,e), A(l,l,e,e)}, 1, 0, 1 }, \
	[OXSet+IC##c] = { "xset" #c,     0, {A(x,x,e,e), A(x,x,e,e)}, 0, 1, 0 },
	ICMPS(X)
#undef X
#define X(c) \
	[OCmps+FC##c] = { "c"    #c "s", 0, {A(s,s,e,e), A(s,s,e,e)}, 1, 0, 1 }, \
	[OCmpd+FC##c] = { "c"    #c "d", 0, {A(d,d,e,e), A(d,d,e,e)}, 1, 0, 1 },
	FCMPS(X)
#undef X

};
#undef A

typedef enum {
	PXXX,
	PLbl,
	PPhi,
	PIns,
	PEnd,
} PState;

enum {
	TXXX = NPubOp,
	TCall,
	TPhi,
	TJmp,
	TJnz,
	TRet,
	TExport,
	TFunc,
	TType,
	TData,
	TAlign,
	TL,
	TW,
	TH,
	TB,
	TD,
	TS,
	TZ,

	TInt,
	TFlts,
	TFltd,
	TTmp,
	TLbl,
	TGlo,
	TTyp,
	TStr,

	TPlus,
	TEq,
	TComma,
	TLParen,
	TRParen,
	TLBrace,
	TRBrace,
	TNL,
	TEOF,
};


static FILE *inf;
static char *inpath;
static int thead;
static struct {
	char chr;
	double fltd;
	float flts;
	int64_t num;
	char *str;
} tokval;
static int lnum;

static Fn *curf;
static Phi **plink;
static Blk **bmap;
static Blk *curb;
static Blk **blink;
static int nblk;
static int rcls;
static int ntyp;


void
err(char *s, ...)
{
	va_list ap;

	va_start(ap, s);
	fprintf(stderr, "%s:%d: ", inpath, lnum);
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	exit(1);
}

static int
lex()
{
	static struct {
		char *str;
		int tok;
	} tmap[] = {
		{ "call", TCall },
		{ "phi", TPhi },
		{ "jmp", TJmp },
		{ "jnz", TJnz },
		{ "ret", TRet },
		{ "export", TExport },
		{ "function", TFunc },
		{ "type", TType },
		{ "data", TData },
		{ "align", TAlign },
		{ "l", TL },
		{ "w", TW },
		{ "h", TH },
		{ "b", TB },
		{ "d", TD },
		{ "s", TS },
		{ "z", TZ },
		{ "loadw", OLoad }, /* for convenience */
		{ "loadl", OLoad },
		{ "loads", OLoad },
		{ "loadd", OLoad },
		{ "alloc1", OAlloc },
		{ "alloc2", OAlloc },
		{ 0, TXXX }
	};
	static char tok[NString];
	int c, i;
	int t;

	do
		c = fgetc(inf);
	while (isblank(c));
	t = TXXX;
	tokval.chr = c;
	switch (c) {
	case EOF:
		return TEOF;
	case ',':
		return TComma;
	case '(':
		return TLParen;
	case ')':
		return TRParen;
	case '{':
		return TLBrace;
	case '}':
		return TRBrace;
	case '=':
		return TEq;
	case '+':
		return TPlus;
	case 's':
		if (fscanf(inf, "_%f", &tokval.flts) != 1)
			break;
		return TFlts;
	case 'd':
		if (fscanf(inf, "_%lf", &tokval.fltd) != 1)
			break;
		return TFltd;
	case '%':
		t = TTmp;
		goto Alpha;
	case '@':
		t = TLbl;
		goto Alpha;
	case '$':
		t = TGlo;
		goto Alpha;
	case ':':
		t = TTyp;
		goto Alpha;
	case '#':
		while ((c=fgetc(inf)) != '\n' && c != EOF)
			;
	case '\n':
		lnum++;
		return TNL;
	}
	if (isdigit(c) || c == '-' || c == '+') {
		ungetc(c, inf);
		if (fscanf(inf, "%"SCNd64, &tokval.num) != 1)
			err("invalid integer literal");
		return TInt;
	}
	if (c == '"') {
		tokval.str = vnew(0, 1);
		for (i=0;; i++) {
			c = fgetc(inf);
			if (c == EOF)
				err("unterminated string");
			vgrow(&tokval.str, i+1);
			if (c == '"')
			if (!i || tokval.str[i-1] != '\\') {
				tokval.str[i] = 0;
				return TStr;
			}
			tokval.str[i] = c;
		}
	}
	if (0)
Alpha:		c = fgetc(inf);
	if (!isalpha(c) && c != '.' && c != '_')
		err("lexing failure: invalid character %c (%d)", c, c);
	i = 0;
	do {
		if (i >= NString-1)
			err("identifier too long");
		tok[i++] = c;
		c = fgetc(inf);
	} while (isalpha(c) || c == '$' || c == '.' || c == '_' || isdigit(c));
	tok[i] = 0;
	ungetc(c, inf);
	tokval.str = tok;
	if (t != TXXX) {
		return t;
	}
	for (i=0; i<NPubOp; i++)
		if (opdesc[i].name)
		if (strcmp(tok, opdesc[i].name) == 0)
			return i;
	for (i=0; tmap[i].str; i++)
		if (strcmp(tok, tmap[i].str) == 0)
			return tmap[i].tok;
	err("unknown keyword %s", tokval.str);
	return TXXX;
}

static int
peek()
{
	if (thead == TXXX)
		thead = lex();
	return thead;
}

static int
next()
{
	int t;

	t = peek();
	thead = TXXX;
	return t;
}

static int
nextnl()
{
	int t;

	while ((t = next()) == TNL)
		;
	return t;
}

static void
expect(int t)
{
	static char *ttoa[] = {
		[TLbl] = "label",
		[TComma] = ",",
		[TEq] = "=",
		[TNL] = "newline",
		[TLParen] = "(",
		[TRParen] = ")",
		[TLBrace] = "{",
		[TRBrace] = "}",
		[TEOF] = 0,
	};
	char buf[128], *s1, *s2;
	int t1;

	t1 = next();
	if (t == t1)
		return;
	s1 = ttoa[t] ? ttoa[t] : "??";
	s2 = ttoa[t1] ? ttoa[t1] : "??";
	sprintf(buf, "%s expected, got %s instead", s1, s2);
	err(buf);
}

static Ref
tmpref(char *v)
{
	int t;

	for (t=Tmp0; t<curf->ntmp; t++)
		if (strcmp(v, curf->tmp[t].name) == 0)
			return TMP(t);
	newtmp(0, Kw, curf);
	strcpy(curf->tmp[t].name, v);
	return TMP(t);
}

static Ref
parseref()
{
	Con c;
	int i;

	memset(&c, 0, sizeof c);
	switch (next()) {
	case TTmp:
		return tmpref(tokval.str);
	case TInt:
		c.type = CBits;
		c.bits.i = tokval.num;
		goto Look;
	case TFlts:
		c.type = CBits;
		c.bits.s = tokval.flts;
		c.flt = 1;
		goto Look;
	case TFltd:
		c.type = CBits;
		c.bits.d = tokval.fltd;
		c.flt = 2;
		goto Look;
	case TGlo:
		c.type = CAddr;
		strcpy(c.label, tokval.str);
	Look:
		for (i=0; i<curf->ncon; i++)
			if (curf->con[i].type == c.type
			&& curf->con[i].bits.i == c.bits.i
			&& strcmp(curf->con[i].label, c.label) == 0)
				return CON(i);
		vgrow(&curf->con, ++curf->ncon);
		curf->con[i] = c;
		return CON(i);
	default:
		return R;
	}
}

static int
parsecls(int *tyn)
{
	int i;

	switch (next()) {
	default:
		err("invalid class specifier");
	case TTyp:
		for (i=0; i<ntyp; i++)
			if (strcmp(tokval.str, typ[i].name) == 0) {
				*tyn = i;
				return 4;
			}
		err("undefined type");
	case TW:
		return Kw;
	case TL:
		return Kl;
	case TS:
		return Ks;
	case TD:
		return Kd;
	}
}

static void
parserefl(int arg)
{
	int k, t, ty;
	Ref r;

	expect(TLParen);
	if (peek() == TRParen) {
		next();
		return;
	}
	for (;;) {
		if (curi - insb >= NIns)
			err("too many instructions (1)");
		k = parsecls(&ty);
		r = parseref();
		if (req(r, R))
			err("invalid reference argument");
		if (!arg && rtype(r) != RTmp)
			err("invalid function parameter");
		if (k == 4)
			if (arg)
				*curi = (Ins){OArgc, R, {TYPE(ty), r}, Kl};
			else
				*curi = (Ins){OParc, r, {TYPE(ty)}, Kl};
		else
			if (arg)
				*curi = (Ins){OArg, R, {r}, k};
			else
				*curi = (Ins){OPar, r, {R}, k};
		if (!arg)
			curf->tmp[r.val].cls = curi->cls;
		curi++;
		t = next();
		if (t == TRParen)
			break;
		if (t != TComma)
			err(", or ) expected");
	}
}

static Blk *
findblk(char *name)
{
	int i;

	for (i=0; i<nblk; i++)
		if (strcmp(bmap[i]->name, name) == 0)
			return bmap[i];
	vgrow(&bmap, ++nblk);
	bmap[i] = blknew();
	bmap[i]->id = i;
	strcpy(bmap[i]->name, name);
	return bmap[i];
}

static void
closeblk()
{
	curb->nins = curi - insb;
	idup(&curb->ins, insb, curb->nins);
	blink = &curb->link;
	curi = insb;
}

static PState
parseline(PState ps)
{
	Ref arg[NPred] = {R};
	Blk *blk[NPred];
	Phi *phi;
	Ref r;
	Blk *b;
	int t, op, i, k, ty;

	t = nextnl();
	if (ps == PLbl && t != TLbl && t != TRBrace)
		err("label or } expected");
	switch (t) {
	default:
		if (isstore(t)) {
			/* operations without result */
			r = R;
			k = 0;
			op = t;
			goto DoOp;
		}
		err("label, instruction or jump expected");
	case TRBrace:
		return PEnd;
	case TTmp:
		break;
	case TLbl:
		b = findblk(tokval.str);
		if (b->jmp.type != JXXX)
			err("multiple definitions of block");
		if (curb && curb->jmp.type == JXXX) {
			closeblk();
			curb->jmp.type = JJmp;
			curb->s1 = b;
		}
		*blink = b;
		curb = b;
		plink = &curb->phi;
		expect(TNL);
		return PPhi;
	case TRet:
		curb->jmp.type = (int[]){
			JRetw, JRetl,
			JRets, JRetd,
			JRetc, JRet0
		}[rcls];
		if (rcls < 5) {
			r = parseref();
			if (req(r, R))
				err("return value expected");
			curb->jmp.arg = r;
		}
		goto Close;
	case TJmp:
		curb->jmp.type = JJmp;
		goto Jump;
	case TJnz:
		curb->jmp.type = JJnz;
		r = parseref();
		if (req(r, R))
			err("invalid argument for jnz jump");
		curb->jmp.arg = r;
		expect(TComma);
	Jump:
		expect(TLbl);
		curb->s1 = findblk(tokval.str);
		if (curb->jmp.type != JJmp) {
			expect(TComma);
			expect(TLbl);
			curb->s2 = findblk(tokval.str);
		}
		if (curb->s1 == curf->start || curb->s2 == curf->start)
			err("invalid jump to the start node");
	Close:
		expect(TNL);
		closeblk();
		return PLbl;
	}
	r = tmpref(tokval.str);
	expect(TEq);
	k = parsecls(&ty);
	op = next();
DoOp:
	if (op == TPhi) {
		if (ps != PPhi)
			err("unexpected phi instruction");
		op = -1;
	}
	if (op == TCall) {
		arg[0] = parseref();
		parserefl(1);
		expect(TNL);
		op = OCall;
		if (k == 4) {
			k = Kl;
			arg[1] = TYPE(ty);
		} else
			arg[1] = R;
		goto Ins;
	}
	if (k == 4)
		err("size class must be w, l, s, or d");
	if (op >= NPubOp)
		err("invalid instruction");
	i = 0;
	if (peek() != TNL)
		for (;;) {
			if (i == NPred)
				err("too many arguments");
			if (op == -1) {
				expect(TLbl);
				blk[i] = findblk(tokval.str);
			}
			arg[i] = parseref();
			if (req(arg[i], R))
				err("invalid instruction argument");
			i++;
			t = peek();
			if (t == TNL)
				break;
			if (t != TComma)
				err(", or end of line expected");
			next();
		}
	next();
Ins:
	if (!req(r, R))
		curf->tmp[r.val].cls = k;
	if (op != -1) {
		if (curi - insb >= NIns)
			err("too many instructions (2)");
		curi->op = op;
		curi->cls = k;
		curi->to = r;
		curi->arg[0] = arg[0];
		curi->arg[1] = arg[1];
		curi++;
		return PIns;
	} else {
		phi = alloc(sizeof *phi);
		phi->to = r;
		phi->cls = k;
		memcpy(phi->arg, arg, i * sizeof arg[0]);
		memcpy(phi->blk, blk, i * sizeof blk[0]);
		phi->narg = i;
		*plink = phi;
		plink = &phi->link;
		return PPhi;
	}
}

static int
oktype(Ref r, int k, Fn *fn)
{
	return rtype(r) != RTmp || fn->tmp[r.val].cls == k
		|| (fn->tmp[r.val].cls == Kl && k == Kw);
}

static void
validate(Fn *fn)
{
	Blk *b;
	Phi *p;
	Ins *i;
	uint n;
	int k;
	BSet pb[1], ppb[1];

	fillpreds(fn);
	bsinit(pb, fn->nblk);
	bsinit(ppb, fn->nblk);
	for (b=fn->start; b; b=b->link) {
		bszero(pb);
		for (n=0; n<b->npred; n++)
			bsset(pb, b->pred[n]->id);
		for (p=b->phi; p; p=p->link) {
			bszero(ppb);
			for (n=0; n<p->narg; n++) {
				k = fn->tmp[p->to.val].cls;
				if (bshas(ppb, p->blk[n]->id))
					err("multiple entries for @%s in phi %%%s",
						p->blk[n]->name,
						fn->tmp[p->to.val].name);
				if (!oktype(p->arg[n], k, fn))
					err("invalid type for operand %%%s in phi %%%s",
						fn->tmp[p->arg[n].val].name,
						fn->tmp[p->to.val].name);
				bsset(ppb, p->blk[n]->id);
			}
			if (!bsequal(pb, ppb))
				err("predecessors not matched in phi %%%s",
					fn->tmp[p->to.val].name);
		}
		for (i=b->ins; i-b->ins < b->nins; i++)
			for (n=0; n<2; n++) {
				k = opdesc[i->op].argcls[n][i->cls];
				if (k == Ke)
					err("invalid instruction type in %s",
						opdesc[i->op].name);
				if (rtype(i->arg[n]) == RType)
					continue;
				if (rtype(i->arg[n]) != -1 && k == Kx)
					err("no %s operand expected in %s",
						n == 1 ? "second" : "first",
						opdesc[i->op].name);
				if (rtype(i->arg[n]) == -1 && k != Kx)
					err("missing %s operand in %s",
						n == 1 ? "second" : "first",
						opdesc[i->op].name);
				if (!oktype(i->arg[n], k, fn))
					err("invalid type for %s operand %%%s in %s",
						n == 1 ? "second" : "first",
						fn->tmp[i->arg[n].val].name,
						opdesc[i->op].name);
			}
		if (isret(b->jmp.type)) {
			if (b->jmp.type == JRetc) {
				if (!oktype(b->jmp.arg, Kl, fn))
					goto JErr;
			} else if (!oktype(b->jmp.arg, b->jmp.type-JRetw, fn))
				goto JErr;
		}
		if (b->jmp.type == JJnz && !oktype(b->jmp.arg, Kw, fn))
		JErr:
			err("invalid type for jump argument %%%s in block @%s",
				fn->tmp[b->jmp.arg.val].name, b->name);
		if (b->s1 && b->s1->jmp.type == JXXX)
			err("block @%s is used undefined", b->s1->name);
		if (b->s2 && b->s2->jmp.type == JXXX)
			err("block @%s is used undefined", b->s2->name);
	}
}

static Fn *
parsefn(int export)
{
	int r;
	PState ps;

	curb = 0;
	nblk = 0;
	curi = insb;
	curf = alloc(sizeof *curf);
	curf->ntmp = 0;
	curf->ncon = 1; /* first constant must be 0 */
	curf->tmp = vnew(curf->ntmp, sizeof curf->tmp[0]);
	curf->con = vnew(curf->ncon, sizeof curf->con[0]);
	for (r=0; r<Tmp0; r++)
		newtmp(0, r < XMM0 ? Kl : Kd, curf);
	bmap = vnew(nblk, sizeof bmap[0]);
	curf->con[0].type = CBits;
	curf->export = export;
	blink = &curf->start;
	curf->retty = -1;
	if (peek() != TGlo)
		rcls = parsecls(&curf->retty);
	else
		rcls = 5;
	if (next() != TGlo)
		err("function name expected");
	strcpy(curf->name, tokval.str);
	parserefl(0);
	if (nextnl() != TLBrace)
		err("function body must start with {");
	ps = PLbl;
	do
		ps = parseline(ps);
	while (ps != PEnd);
	if (!curb)
		err("empty file");
	if (curb->jmp.type == JXXX)
		err("last block misses jump");
	curf->mem = vnew(0, sizeof curf->mem[0]);
	curf->nmem = 0;
	curf->nblk = nblk;
	curf->rpo = 0;
	validate(curf);
	return curf;
}

static void
parsetyp()
{
	Typ *ty;
	int t, n, sz, al, s, a, c, flt;

	if (ntyp >= NTyp)
		err("too many type definitions");
	ty = &typ[ntyp++];
	ty->align = -1;
	if (nextnl() != TTyp ||  nextnl() != TEq)
		err("type name, then = expected");
	strcpy(ty->name, tokval.str);
	t = nextnl();
	if (t == TAlign) {
		if (nextnl() != TInt)
			err("alignment expected");
		for (al=0; tokval.num /= 2; al++)
			;
		ty->align = al;
		t = nextnl();
	}
	if (t != TLBrace)
		err("type body must start with {");
	t = nextnl();
	if (t == TInt) {
		ty->dark = 1;
		ty->size = tokval.num;
		if (ty->align == -1)
			err("dark types need alignment");
		t = nextnl();
	} else {
		ty->dark = 0;
		n = -1;
		sz = 0;
		al = 0;
		for (;;) {
			flt = 0;
			switch (t) {
			default: err("invalid size specifier %c", tokval.chr);
			case TD: flt = 1;
			case TL: s = 8; a = 3; break;
			case TS: flt = 1;
			case TW: s = 4; a = 2; break;
			case TH: s = 2; a = 1; break;
			case TB: s = 1; a = 0; break;
			}
			if (a > al)
				al = a;
			if ((a = sz & (s-1))) {
				a = s - a;
				if (++n < NSeg) {
					/* padding segment */
					ty->seg[n].ispad = 1;
					ty->seg[n].len = a;
				}
			}
			t = nextnl();
			if (t == TInt) {
				c = tokval.num;
				t = nextnl();
			} else
				c = 1;
			while (c-- > 0) {
				if (++n < NSeg) {
					ty->seg[n].isflt = flt;
					ty->seg[n].ispad = 0;
					ty->seg[n].len = s;
				}
				sz += a + s;
			}
			if (t != TComma)
				break;
			t = nextnl();
		}
		if (++n >= NSeg)
			ty->dark = 1;
		else
			ty->seg[n].len = 0;
		if (ty->align == -1)
			ty->align = al;
		else
			al = ty->align;
		a = (1 << al) - 1;
		ty->size = (sz + a) & ~a;
	}
	if (t != TRBrace)
		err("expected closing }");
}

static void
parsedatref(Dat *d)
{
	int t;

	d->isref = 1;
	d->u.ref.nam = tokval.str;
	d->u.ref.off = 0;
	t = peek();
	if (t == TPlus) {
		next();
		if (next() != TInt)
			err("invalid token after offset in ref");
		d->u.ref.off = tokval.num;
	}
}

static void
parsedatstr(Dat *d)
{
	d->isstr = 1;
	d->u.str = tokval.str;
}

static void
parsedat(void cb(Dat *), int export)
{
	char s[NString];
	int t;
	Dat d;

	d.type = DStart;
	d.isstr = 0;
	d.isref = 0;
	d.export = export;
	cb(&d);
	if (nextnl() != TGlo || nextnl() != TEq)
		err("data name, then = expected");
	strcpy(s, tokval.str);
	t = nextnl();
	if (t == TAlign) {
		if (nextnl() != TInt)
			err("alignment expected");
		d.type = DAlign;
		d.u.num = tokval.num;
		cb(&d);
		t = nextnl();
	}
	d.type = DName;
	d.u.str = s;
	cb(&d);

	if (t != TLBrace)
		err("expected data contents in { .. }");
	for (;;) {
		switch (nextnl()) {
		default: err("invalid size specifier %c in data", tokval.chr);
		case TRBrace: goto Done;
		case TL: d.type = DL; break;
		case TW: d.type = DW; break;
		case TH: d.type = DH; break;
		case TB: d.type = DB; break;
		case TS: d.type = DW; break;
		case TD: d.type = DL; break;
		case TZ: d.type = DZ; break;
		}
		t = nextnl();
		do {
			d.isref = 0;
			d.isstr = 0;
			memset(&d.u, 0, sizeof d.u);
			if (t == TFlts)
				d.u.flts = tokval.flts;
			else if (t == TFltd)
				d.u.fltd = tokval.fltd;
			else if (t == TInt)
				d.u.num = tokval.num;
			else if (t == TGlo)
				parsedatref(&d);
			else if (t == TStr)
				parsedatstr(&d);
			else
				err("constant literal expected");
			cb(&d);
			t = nextnl();
		} while (t == TInt || t == TFlts || t == TFltd);
		if (t == TRBrace)
			break;
		if (t != TComma)
			err(", or } expected");
	}
Done:
	d.type = DEnd;
	cb(&d);
}

void
parse(FILE *f, char *path, void data(Dat *), void func(Fn *))
{
	int t, export;

	inf = f;
	inpath = path;
	lnum = 1;
	thead = TXXX;
	ntyp = 0;
	for (;;) {
		export = 0;
		switch (nextnl()) {
		default:
			err("top-level definition expected");
		case TExport:
			export = 1;
			t = nextnl();
			if (t == TFunc) {
		case TFunc:
				func(parsefn(export));
				break;
			}
			else if (t == TData) {
		case TData:
				parsedat(data, export);
				break;
			}
			else
				err("export can only qualify data and function");
		case TType:
			parsetyp();
			break;
		case TEOF:
			return;
		}
	}
}

static void
printcon(Con *c, FILE *f)
{
	switch (c->type) {
	case CUndef:
		break;
	case CAddr:
		fprintf(f, "$%s", c->label);
		if (c->bits.i)
			fprintf(f, "%+"PRIi64, c->bits.i);
		break;
	case CBits:
		if (c->flt == 1)
			fprintf(f, "s_%f", c->bits.s);
		else if (c->flt == 2)
			fprintf(f, "d_%lf", c->bits.d);
		else
			fprintf(f, "%"PRIi64, c->bits.i);
		break;
	}
}

void
printref(Ref r, Fn *fn, FILE *f)
{
	int i;
	Mem *m;

	switch (rtype(r)) {
	case RTmp:
		if (r.val < Tmp0)
			fprintf(f, "R%d", r.val);
		else
			fprintf(f, "%%%s", fn->tmp[r.val].name);
		break;
	case RCon:
		printcon(&fn->con[r.val], f);
		break;
	case RSlot:
		fprintf(f, "S%d", (r.val&(1<<28)) ? r.val-(1<<29) : r.val);
		break;
	case RCall:
		fprintf(f, "%03x", r.val);
		break;
	case RType:
		fprintf(f, ":%s", typ[r.val].name);
		break;
	case RMem:
		i = 0;
		m = &fn->mem[r.val];
		fputc('[', f);
		if (m->offset.type != CUndef) {
			printcon(&m->offset, f);
			i = 1;
		}
		if (!req(m->base, R)) {
			if (i)
				fprintf(f, " + ");
			printref(m->base, fn, f);
			i = 1;
		}
		if (!req(m->index, R)) {
			if (i)
				fprintf(f, " + ");
			fprintf(f, "%d * ", m->scale);
			printref(m->index, fn, f);
		}
		fputc(']', f);
		break;
	}
}

void
printfn(Fn *fn, FILE *f)
{
	static char *jtoa[NJmp] = {
		[JRet0]     = "ret",
		[JRetw]     = "retw",
		[JRetl]     = "retl",
		[JRetc]     = "retc",
		[JRets]     = "rets",
		[JRetd]     = "retd",
		[JJnz]      = "jnz",
		[JXJnp]     = "xjnp",
		[JXJp]      = "xjp",
	#define X(c) [JXJc+IC##c] = "xj" #c,
		ICMPS(X)
	#undef X
	};
	static char prcls[NOp] = {
		[OArg] = 1,
		[OSwap] = 1,
		[OXCmp] = 1,
		[OXTest] = 1,
		[OXDiv] = 1,
		[OXIDiv] = 1,
	};
	static char ktoc[] = "wlsd";
	Blk *b;
	Phi *p;
	Ins *i;
	uint n;

	if (fn->export)
		fprintf(f, "export ");
	fprintf(f, "function $%s() {\n", fn->name);
	for (b=fn->start; b; b=b->link) {
		fprintf(f, "@%s\n", b->name);
		for (p=b->phi; p; p=p->link) {
			fprintf(f, "\t");
			printref(p->to, fn, f);
			fprintf(f, " =%c phi ", ktoc[p->cls]);
			assert(p->narg);
			for (n=0;; n++) {
				fprintf(f, "@%s ", p->blk[n]->name);
				printref(p->arg[n], fn, f);
				if (n == p->narg-1) {
					fprintf(f, "\n");
					break;
				} else
					fprintf(f, ", ");
			}
		}
		for (i=b->ins; i-b->ins < b->nins; i++) {
			fprintf(f, "\t");
			if (!req(i->to, R)) {
				printref(i->to, fn, f);
				fprintf(f, " =%c ", ktoc[i->cls]);
			}
			assert(opdesc[i->op].name);
			fprintf(f, "%s", opdesc[i->op].name);
			if (req(i->to, R) && prcls[i->op])
				fputc(ktoc[i->cls], f);
			if (!req(i->arg[0], R)) {
				fprintf(f, " ");
				printref(i->arg[0], fn, f);
			}
			if (!req(i->arg[1], R)) {
				fprintf(f, ", ");
				printref(i->arg[1], fn, f);
			}
			fprintf(f, "\n");
		}
		switch (b->jmp.type) {
		case JRet0:
		case JRetw:
		case JRetl:
		case JRets:
		case JRetd:
		case JRetc:
			fprintf(f, "\t%s", jtoa[b->jmp.type]);
			if (b->jmp.type != JRet0 || !req(b->jmp.arg, R)) {
				fprintf(f, " ");
				printref(b->jmp.arg, fn, f);
			}
			if (b->jmp.type == JRetc)
				fprintf(f, ", :%s", typ[fn->retty].name);
			fprintf(f, "\n");
			break;
		case JJmp:
			if (b->s1 != b->link)
				fprintf(f, "\tjmp @%s\n", b->s1->name);
			break;
		default:
			fprintf(f, "\t%s ", jtoa[b->jmp.type]);
			if (b->jmp.type == JJnz) {
				printref(b->jmp.arg, fn, f);
				fprintf(f, ", ");
			}
			fprintf(f, "@%s, @%s\n", b->s1->name, b->s2->name);
			break;
		}
	}
	fprintf(f, "}\n");
}
