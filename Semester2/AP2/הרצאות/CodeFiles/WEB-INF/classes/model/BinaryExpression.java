package model;

public abstract class BinaryExpression implements Expression {
	
	protected Expression left;
	protected Expression right;

	public BinaryExpression(Expression left, Expression right) {
		setLeft(left);
		setRight(right);
	}

	@Override
	public abstract double calculate();

	public Expression getLeft() {
		return left;
	}

	public void setLeft(Expression left) {
		this.left = left;
	}

	public Expression getRight() {
		return right;
	}

	public void setRight(Expression right) {
		this.right = right;
	}

}
