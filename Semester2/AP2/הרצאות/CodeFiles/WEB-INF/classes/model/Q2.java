package model;

public class Q2 {

	public Q2() {}
	
	public static double calc(){
		Expression div = new Div(new Number(10), new Number(2));
		Expression minus = new Minus(new Number(3), new Number(4));
		Expression mult = new Mul(new Number(2), minus);
		Expression plus = new Plus(div, mult);
		
		return plus.calculate();
	}

}
