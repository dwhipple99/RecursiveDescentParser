using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

// 
//
// A recursive descent parser using C# and LINQ

//  This parser uses Augmented Backus-Naur Form Grammer
//  Each grammar is made up of rules called productions
//
//  Terminals           full-stop="."
//  Or                  logical-constant = "FALSE"/"TRUE"
//  Adjacent Symbols    Two symbols seperated by a space                        
//                      fractional-part=full-stop digit-sequence
//  Optional            A symbol or terminal that is optional surrounded by [] 
//                      exponent-part = exponent-character [ sign ] digit-sequence
//  Zero or More        If a symbol is preceded by an asterisk (*), zero or more of those symbols can occur
//  N or More           If a symbol is preceded by a number followed by an asterisk
//                      digit-sequence = 1*decimal-digit
//  Exactly N Symbols   If a symbol is preceded by a number
//                      escaped-double-quote = 2double-quote
//  N to M Symbols      and-params = "(" (argument-expression / (argument 1*254("," argument))) ")"
//  Grouped Symbols     Symbols in a production can be grouped by parentheses
//                      constant-list-rows = constant-list-row *(semicolon constant-list-row)
//  Exceptions and Special Rules
//                      In some places, the grammar defines some special rules in text

// My approach for coding the recursive descent parser will be to paste the grammar rule directly 
// into the class that implements the rule as a C# comment.  This makes it very easy to correlate 
// the grammar to the code that implements the rule.

//    Operator  Precedence
//      +           1
//      -           2
//      *           2
//      /           2
//      ^           3

namespace SimpleParser
{
    public abstract class Symbol
    {
        public List<Symbol> ConstituentSymbols { get; set; }
        public override string ToString()
        {
            string s = ConstituentSymbols.Select(ct => ct.ToString()).StringConcatenate();
            return s;
        }
        public Symbol(params Object[] symbols)
        {
            List<Symbol> ls = new List<Symbol>();
            foreach (var item in symbols)
            {
                if (item is Symbol)
                    ls.Add((Symbol)item);
                else if (item is IEnumerable<Symbol>)
                    foreach (var item2 in (IEnumerable<Symbol>)item)
                        ls.Add(item2);
                else
                    // If this error is thrown, the parser is coded incorrectly.
                    throw new ParserException("Internal error");
            }
            ConstituentSymbols = ls;
        }
        public Symbol() { }
    }

    public class Formula : Symbol
    {
        public static Formula Produce(IEnumerable<Symbol> symbols)
        {
            // formula = expression

            Expression e = Expression.Produce(symbols);
            return e == null ? null : new Formula(e);
        }
        public Formula(params Object[] symbols) : base(symbols) { }
    }

    public class Expression : Symbol
    {
        public static Expression Produce(IEnumerable<Symbol> symbols)
        {
            // expression = *whitespace nospace-expression *whitespace

            int whiteSpaceBefore = symbols.TakeWhile(s => s is WhiteSpace).Count();
            int whiteSpaceAfter = symbols.Reverse().TakeWhile(s => s is WhiteSpace).Count();
            IEnumerable<Symbol> noSpaceSymbolList = symbols
                .Skip(whiteSpaceBefore)
                .SkipLast(whiteSpaceAfter)
                .ToList();
            NospaceExpression n = NospaceExpression.Produce(noSpaceSymbolList);
            if (n != null)
                return new Expression(
                    Enumerable.Repeat(new WhiteSpace(), whiteSpaceBefore),
                    n,
                    Enumerable.Repeat(new WhiteSpace(), whiteSpaceAfter));
            return null;
        }

        public Expression(params Object[] symbols) : base(symbols) { }
    }

    public class NospaceExpression : Symbol
    {
        public static Dictionary<string, int> OperatorPrecedence = new Dictionary<string, int>()
        {
            { "^", 3 },
            { "*", 2 },
            { "/", 2 },
            { "+", 1 },
            { "-", 1 },
        };

        public static NospaceExpression Produce(IEnumerable<Symbol> symbols)
        {
            // nospace-expression = open-parenthesis expression close-parenthesis
            //         / numerical-constant
            //         / prefix-operator expression
            //         / expression infix-operator expression

            if (!symbols.Any())
                return null;

            if (symbols.First() is OpenParenthesis && symbols.Last() is CloseParenthesis)
            {
                Expression e = Expression.Produce(symbols.Skip(1).SkipLast(1));
                if (e != null)
                    return new NospaceExpression(new OpenParenthesis(), e, new CloseParenthesis());
            }

            // expression, infix-operator, expression
            var z = symbols.Rollup(0, (t, d) =>
            {
                if (t is OpenParenthesis)
                    return d + 1;
                if (t is CloseParenthesis)
                    return d - 1;
                return d;
            });
            var symbolsWithIndex = symbols.Select((s, i) => new
            {
                Symbol = s,
                Index = i,
            });
            var z2 = symbolsWithIndex.Zip(z, (v1, v2) => new
            {
                SymbolWithIndex = v1,
                Depth = v2,
            });
            var operatorList = z2
                .Where(x => x.Depth == 0 &&
                    x.SymbolWithIndex.Index != 0 &&
                    InfixOperator.Produce(x.SymbolWithIndex.Symbol) != null)
                .ToList();
            if (operatorList.Any())
            {
                int minPrecedence = operatorList
                    .Select(o2 => OperatorPrecedence[o2.SymbolWithIndex.Symbol.ToString()]).Min();
                var op = operatorList
                    .Last(o2 => OperatorPrecedence[o2.SymbolWithIndex.Symbol.ToString()] == minPrecedence);
                if (op != null)
                {
                    var expressionTokenList1 = symbols.TakeWhile(t => t != op.SymbolWithIndex.Symbol);
                    Expression e1 = Expression.Produce(expressionTokenList1);
                    if (e1 == null)
                        throw new ParserException("Invalid expression");
                    var expressionTokenList2 = symbols
                        .SkipWhile(t => t != op.SymbolWithIndex.Symbol).Skip(1);
                    Expression e2 = Expression.Produce(expressionTokenList2);
                    if (e2 == null)
                        throw new ParserException("Invalid expression");
                    InfixOperator io = new InfixOperator(op.SymbolWithIndex.Symbol);
                    return new NospaceExpression(e1, io, e2);
                }
            }

            NumericalConstant n = NumericalConstant.Produce(symbols);
            if (n != null)
                return new NospaceExpression(n);

            PrefixOperator p = PrefixOperator.Produce(symbols.FirstOrDefault());
            if (p != null)
            {
                Expression e = Expression.Produce(symbols.Skip(1));
                if (e != null)
                    return new NospaceExpression(p, e);
            }

            return null;
        }

        public NospaceExpression(params Object[] symbols) : base(symbols) { }
    }

    public class NumericalConstant : Symbol
    {
        public static NumericalConstant Produce(IEnumerable<Symbol> symbols)
        {
            // numerical-constant = [neg-sign] significand-part

            SignificandPart s = SignificandPart.Produce(symbols);
            if (s != null)
                return new NumericalConstant(s);
            NegSign n = NegSign.Produce(symbols.First());
            if (n != null)
            {
                SignificandPart s2 = SignificandPart.Produce(symbols.Skip(1));
                if (s2 != null)
                    return new NumericalConstant(n, s2);
            }
            return null;
        }
        public NumericalConstant(params Object[] symbols) : base(symbols) { }
    }

    public class SignificandPart : Symbol
    {
        public static SignificandPart Produce(IEnumerable<Symbol> symbols)
        {
            // significand-part = whole-number-part [fractional-part] / fractional-part

            FractionalPart f;
            f = FractionalPart.Produce(symbols);
            if (f != null)
                return new SignificandPart(f);
            IEnumerable<Symbol> s = null;
            WholeNumberPart w = WholeNumberPart.Produce(symbols, out s);
            if (w != null)
            {
                if (!s.Any())
                    return new SignificandPart(w);
                f = FractionalPart.Produce(s);
                if (f != null)
                    return new SignificandPart(w, f);
            }
            return null;
        }
        public SignificandPart(params Object[] symbols) : base(symbols) { }
    }

    public class WholeNumberPart : Symbol
    {
        public static WholeNumberPart Produce(IEnumerable<Symbol> symbols,
            out IEnumerable<Symbol> symbolsToProcess)
        {
            // whole-number-part = digit-sequence

            IEnumerable<Symbol> s = null;
            DigitSequence d = DigitSequence.Produce(symbols, out s);
            if (d != null)
            {
                symbolsToProcess = s;
                return new WholeNumberPart(d);
            }
            symbolsToProcess = null;
            return null;
        }
        public WholeNumberPart(params Object[] symbols) : base(symbols) { }
    }

    public class FractionalPart : Symbol
    {
        public static FractionalPart Produce(IEnumerable<Symbol> symbols)
        {
            // fractional-part = full-stop digit-sequence

            if (!symbols.Any())
                return null;
            if (symbols.First() is FullStop)
            {
                IEnumerable<Symbol> s = null;
                DigitSequence d = DigitSequence.Produce(symbols.Skip(1), out s);
                if (d == null || s.Any())
                    return null;
                return new FractionalPart(new FullStop(), d);
            }
            return null;
        }
        public FractionalPart(params Object[] symbols) : base(symbols) { }
    }

    public class DigitSequence : Symbol
    {
        public static DigitSequence Produce(IEnumerable<Symbol> symbols,
            out IEnumerable<Symbol> symbolsToProcess)
        {
            // digit-sequence = 1*decimal-digit

            IEnumerable<Symbol> digits = symbols.TakeWhile(s => s is DecimalDigit);
            if (digits.Any())
            {
                symbolsToProcess = symbols.Skip(digits.Count());
                return new DigitSequence(digits);
            }
            symbolsToProcess = null;
            return null;
        }
        public DigitSequence(params Object[] symbols) : base(symbols) { }
    }

    public class NegSign : Symbol
    {
        public static NegSign Produce(Symbol symbol)
        {
            // neg-sign = minus

            if (symbol is Minus)
                return new NegSign(symbol);
            return null;
        }
        public NegSign(params Object[] symbols) : base(symbols) { }
    }

    public class PrefixOperator : Symbol
    {
        public static PrefixOperator Produce(Symbol symbol)
        {
            // prefix-operator = plus / minus

            if (symbol is Plus || symbol is Minus)
                return new PrefixOperator(symbol);
            return null;
        }
        public PrefixOperator(params Object[] symbols) : base(symbols) { }
    }

    public class InfixOperator : Symbol
    {
        public static InfixOperator Produce(Symbol symbol)
        {
            // infix-operator = caret / asterisk / forward-slash / plus / minus

            if (symbol is Plus || symbol is Minus || symbol is Asterisk || symbol is ForwardSlash
                || symbol is Caret)
                return new InfixOperator(symbol);
            return null;
        }
        public InfixOperator(params Object[] symbols) : base(symbols) { }
    }

    public class DecimalDigit : Symbol
    {
        private string CharacterValue;
        public override string ToString() { return CharacterValue; }
        public DecimalDigit(char c) { CharacterValue = c.ToString(); }
    }

    public class WhiteSpace : Symbol
    {
        public override string ToString() { return " "; }
        public WhiteSpace() { }
    }

    public class Plus : Symbol
    {
        public override string ToString() { return "+"; }
        public Plus() { }
    }

    public class Minus : Symbol
    {
        public override string ToString() { return "-"; }
        public Minus() { }
    }

    public class Asterisk : Symbol
    {
        public override string ToString() { return "*"; }
        public Asterisk() { }
    }

    public class ForwardSlash : Symbol
    {
        public override string ToString() { return "/"; }
        public ForwardSlash() { }
    }

    public class Caret : Symbol
    {
        public override string ToString() { return "^"; }
        public Caret() { }
    }

    public class FullStop : Symbol
    {
        public override string ToString() { return "."; }
        public FullStop() { }
    }

    public class OpenParenthesis : Symbol
    {
        public override string ToString() { return "("; }
        public OpenParenthesis() { }
    }

    public class CloseParenthesis : Symbol
    {
        public override string ToString() { return ")"; }
        public CloseParenthesis() { }
    }

    public class SimpleFormulaParser
    {
        public static Symbol ParseFormula(string s)
        {
            IEnumerable<Symbol> symbols = s.Select(c =>
            {
                switch (c)
                {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        return new DecimalDigit(c);
                    case ' ':
                        return new WhiteSpace();
                    case '+':
                        return new Plus();
                    case '-':
                        return new Minus();
                    case '*':
                        return new Asterisk();
                    case '/':
                        return new ForwardSlash();
                    case '^':
                        return new Caret();
                    case '.':
                        return new FullStop();
                    case '(':
                        return new OpenParenthesis();
                    case ')':
                        return new CloseParenthesis();
                    default:
                        return (Symbol)null;
                }
            });
#if false
            if (symbols.Any())
            {
                Console.WriteLine("Terminal Symbols");
                Console.WriteLine("================");
                foreach (var terminal in symbols)
                    Console.WriteLine("{0} >{1}<", terminal.GetType().Name.ToString(),
                        terminal.ToString());
                Console.WriteLine();
            }
#endif
            Formula formula = Formula.Produce(symbols);
            if (formula == null)
                throw new ParserException("Invalid formula");
            return formula;
        }

        public static void DumpSymbolRecursive(StringBuilder sb, Symbol symbol, int depth)
        {
            sb.Append(string.Format("{0}{1} >{2}<",
                "".PadRight(depth * 2),
                symbol.GetType().Name.ToString(),
                symbol.ToString())).Append(Environment.NewLine);
            if (symbol.ConstituentSymbols != null)
                foreach (var childSymbol in symbol.ConstituentSymbols)
                    DumpSymbolRecursive(sb, childSymbol, depth + 1);
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            string[] sampleValidFormulas = new[] {
                "1+((2+3)*4)^5",
                "1+2-3*4/5^6",
                "(1+2)/3",
                "  (1+3)  ",
                "-123",
                "1+2*(-3)",
                "1+2*( - 3)",
                "12.34",
                ".34",
                "-123+456",
                "  (  123 + 456 )  ",
                "-.34",
                "-12.34",
                "-(123+456)",
            };

            string[] sampleInvalidFormulas = new[] {
                "-(123+)",
                "-(*123)",
                "*123",
                "*123a",
                "1.",
                "--1",
            };

            StringBuilder sb = new StringBuilder();
            foreach (var formula in sampleValidFormulas)
            {
                Symbol f = SimpleFormulaParser.ParseFormula(formula);
                SimpleFormulaParser.DumpSymbolRecursive(sb, f, 0);
                sb.Append("==================================" + Environment.NewLine);
            }
            foreach (var formula in sampleInvalidFormulas)
            {
                bool exceptionThrown = false;
                try
                {
                    Symbol f = SimpleFormulaParser.ParseFormula(formula);
                }
                catch (ParserException e)
                {
                    exceptionThrown = true;
                    sb.Append(String.Format("Parsing >{0}< Exception: {1}", formula, e.Message) +
                        Environment.NewLine);
                }
                if (!exceptionThrown)
                    sb.Append(String.Format("Parsing >{0}< Should have thrown exception, but did not",
                        formula) + Environment.NewLine);
            }
            Console.WriteLine(sb.ToString());
        }
    }

    public class ParserException : Exception
    {
        public ParserException(string message) : base(message) { }
    }

    public static class MyExtensions
    {
        public static IEnumerable<T> SkipLast<T>(this IEnumerable<T> source,
            int count)
        {
            Queue<T> saveList = new Queue<T>();
            int saved = 0;
            foreach (T item in source)
            {
                if (saved < count)
                {
                    saveList.Enqueue(item);
                    ++saved;
                    continue;
                }
                saveList.Enqueue(item);
                yield return saveList.Dequeue();
            }
            yield break;
        }

        public static string StringConcatenate(this IEnumerable<string> source)
        {
            StringBuilder sb = new StringBuilder();
            foreach (string s in source)
                sb.Append(s);
            return sb.ToString();
        }

        public static string StringConcatenate<T>(
            this IEnumerable<T> source,
            Func<T, string> projectionFunc)
        {
            return source.Aggregate(new StringBuilder(),
                (s, i) => s.Append(projectionFunc(i)),
                s => s.ToString());
        }

        public static IEnumerable<TResult> Rollup<TSource, TResult>(
            this IEnumerable<TSource> source,
            TResult seed,
            Func<TSource, TResult, TResult> projection)
        {
            TResult nextSeed = seed;
            foreach (TSource src in source)
            {
                TResult projectedValue = projection(src, nextSeed);
                nextSeed = projectedValue;
                yield return projectedValue;
            }
        }
    }
}
