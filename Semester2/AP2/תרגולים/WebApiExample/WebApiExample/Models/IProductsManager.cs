using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WebApiExample.Models
{
    public interface IProductsManager
    {
        IEnumerable<Product> GetAllProducts();
        Product GetProductById(int id);
        void AddProduct(Product p);
        void UpdateProduct(Product p);
        void DeleteProduct(int id);
    }
}
