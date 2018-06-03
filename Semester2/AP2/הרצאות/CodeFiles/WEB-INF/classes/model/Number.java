package model;

public class Number implements Expression {

	private double value;
	
	public Number(double val) {
		setValue(val);
	}

	@Override
	public double calculate() {
		return this.value;
	}
	
	public double getValue() {
		return this.value;
	}

	private void setValue(double value) {
		this.value = value;
	}

}
