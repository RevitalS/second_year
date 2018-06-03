package controller;

import java.io.IOException;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import model.ResultBean;
import model.SYalgorithm;

/**
 * Servlet implementation class MyCalculator
 */
@WebServlet("/MyCalculator2")
public class MyCalculator2 extends HttpServlet {
	private static final long serialVersionUID = 1L;
       
    /**
     * @see HttpServlet#HttpServlet()
     */
    public MyCalculator2() {
        super();
        // TODO Auto-generated constructor stub
    }

	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// TODO Auto-generated method stub
		response.getWriter().append("Served at: ").append(request.getContextPath());
	}

	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// the request
		String problem=request.getParameter("problem");
		// who can fulfil it in the model
		ResultBean rb=new ResultBean(SYalgorithm.calc(problem));
		// return the result
		request.setAttribute("resultBean", rb);
		RequestDispatcher rd=request.getRequestDispatcher("result.jsp");  
        rd.forward(request, response); 		
	}

}
