using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace WebApiExample.Models
{
    public class ProductsManager : IProductsManager
    {
        private static List<Product> products = new List<Product>()
        {
            new Product { Id = 1, Name = "laptop", Price = 1000 },
            new Product { Id = 2, Name = "tablet", Price = 200 },
            new Product { Id = 3, Name = "smartphone", Price = 1200 },
        };

        public void AddProduct(Product p)
        {
            products.Add(p);
        }

        public void DeleteProduct(int id)
        {
            Product p = products.Where(x => x.Id == id).FirstOrDefault();
            if (p == null)
                throw new Exception("product not found");
            products.Remove(p);
        }

        public IEnumerable<Product> GetAllProducts()
        {
            return products;
        }

        public Product GetProductById(int id)
        {
            Product p = products.Where(x => x.Id == id).FirstOrDefault();
            return p;
        }

        public void UpdateProduct(Product p)
        {
            Product prod = products.Where(x => x.Id == p.Id).FirstOrDefault();
            prod.Name = p.Name;
            prod.Price = p.Price;
        }
    }
}