#pragma once

#include <memory>
#include "Product.h"

struct CartItem
{
    // Accept shared_ptr when creating a CartItem…
    CartItem(const std::shared_ptr<Product>& p, int q)
        : product(p), Quantity(q) {
    }

    void ChangeAmount(int a) {
        Quantity = a;
    }

    // …but store it as a weak_ptr (non-owning)
    std::weak_ptr<Product> product;
    int Quantity;
};
