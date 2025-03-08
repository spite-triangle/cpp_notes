# 表单


# 概念

**表单 `Form`** : 表单用于实现用户在 web 界面上填写数据信息，例如添加任务、账号登陆、账号注册等。网页表单是 Web 开发的一项基本功能，Django5 的表单功能主要分为两类
- `django.forms.Form` ，基础的表单功能
- `django.forms.ModelForm`，结合模型所生成的数据表单




# Form 


1. 定义 `forms.py` 表单


```python
from django.forms import ModelForm, Form
from .models import StudentInfo


# NOTE - forms 控件还有其他样式，详情查手册或 API
class BookInfoForm(Form):
    # 文本输入框
    bookName = forms.CharField(
        label='图书名称', 
        max_length=20, 
        required=True, # 必填
        widget=widgets.TextInput(
            attrs={
                    'placeholder': '请输入图书名称', # 提示
                    'class': 'form-control'         # 便于 css 样式添加
                }
        ), 
        initial='测试数据')

    # 数字输入框
    price = forms.FloatField(
        label='图书价格',
        widget=widgets.NumberInput(
            attrs={
                    'placeholder': '请输入图书价格', 
                    'class': 'form-control'
                }
        ))
    
    # 日期选择
    publishDate = forms.DateField(
        label='出版日期',
        widget=widgets.DateInput(
            attrs={
                    'placeholder': '请输入出版日期',
                    'class': 'form-control',
                    'type': 'date'
                }
        ))

    # 下拉框
    # 获取图书类型并且将数据集转换为元组，然后放入下拉框
    choices = BookTypeInfo.objects.all().values('id', 'bookTypeName')
    choices = [(item['id'], item['bookTypeName']) for item in choices]
    bookType_id = forms.ChoiceField(
        label='图书类型', 
        choices=choices,
        widget=widgets.Select(
            attrs={
                'placeholder': '请输入图书类型', 
                'class': 'form-control'
            }
        ))

```

2. 添加路由 `views.py`

```python
from .forms import BookInfoForm
from .models import BookInfo

def add_form(request):
    form = BookInfoForm()
    context = {
            'title': '图书添加‘,
            'form' : form
        }
    return render('book/add_form.html', context = context)

def add_book(request):
    # 定义数据实例
    book = BookInfo()
    book.id = request.POST.get("id")
    book.bookName = request.POST.get("bookName")
    book.publishDate = request.POST.get("publishDate")
    book.price = request.POST.get("price")

    # NOTE - 在实际数据库中外键字段名是 `bookType_id`
    book.bookType_id = request.POST.get("bookType_id")

    # 判断 book.id 是否存在
    # - 存在，修改
    # - 不存在，新增
    book.save()

    # 数据添加后，获取新增数据的主键id
    book.id
    return HttpResponse('ok')
```

3. 定义模板界面 `add_form.html`

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>{{ title }}</title>
</head>
<body>
<h1>{{ title }}</h1>
<style>
    .form-control{width: 100px;}
</style>

<form action="/book/add_book" method="post">
    {% csrf_token %}
    <table>
            <!-- 
                - as_table: 将表单生成为 <table> 的目录项
                - as_ul : 表单字段以HTML的 <ul> 标签生成网页表单
                - as_p : 将表单字段以HTML的 <p> 标签生成网页表单
             -->
            {{ form.as_table }}
    </table>
</select><br>
    <input  type="submit" value="提交">
</form>
</body>
</html>
```

# ModelForm

1. 定义 `forms.py` 表单

```python
class BookInfoModelForm(ModelForm):
    class Meta:
        model = BookInfo
        fields = '__all__'

        widgets = {
            'bookName': forms.TextInput(
                attrs={
                    'class': 'form-control',
                    'placeholder': '请输入图书名称',
                    'id': 'bookName'
                }
            ),
            'price': forms.NumberInput(
                attrs={
                    'class': 'form-control', 
                    'placeholder': '请输入图书价格'
                }
            ),
            'publishDate': forms.DateInput(
                attrs={
                    'class': 'form-control', 
                    'type': 'date', 
                    'placeholder': '请输入出版日期'
                }
            ),

            # 直接适配外键关联对象，且下拉框返回值将直接是 BookInfoType 的主键值
            'bookType': forms.Select(
                attrs={
                    'class': 'form-control', 
                    'placeholder': '请输入图书类型'
                }
            )
        }

        labels = {
            'bookName': '图书名称',
            'price': '图书价格',
            'publishDate': '出版日期',
            'bookType': '图书类型'
        }


```

2. 定义 `views.py`

```python
from .forms import BookInfoModelForm

def add_model_form(request):
    form = BookInfoModelForm()
    content_value = {'title': '添加图书3', 'form': form}

    # html 不用变
    return render(request, 'book/add_form.html', context=content_value)

def add_book_model(request):
    book = BookInfo()
    book.bookName = request.POST.get('bookName')
    book.price = request.POST.get('price')
    book.publishDate = request.POST.get('publishDate')
    # NOTE - BookInfoModelForm 已经将 'bookType' 对应到了 BookInfoType 的主键值
    #        不用使用 'bookType_id' 获取主键值了
    book.bookType_id = request.POST.get('bookType')
    book.save()
    return bookList(request)
————————————————

                            版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。
                        
原文链接：https://blog.csdn.net/sishihao/article/details/140708066
```

3. `BookInfo.bookType` 是 `BookInfoType` 类对象，因此 `BookInfo.bookType` 打印值不是`bookTypeName` 值，还需修改 `BookTypeInfo` 

```python
class BookTypeInfo(models.Model):
    id = models.AutoField(primary_key=True)
    bookTypeName = models.CharField(max_length=20)
    class Meta:
        db_table = "t_bookType"
        verbose_name = "图书类别"
    
    def __str__(self):
        """
        定义返回当前对象的字符串表示形式。
        该方法被调用时，会返回bookTypeName属性的值，用于表示这个对象的名称。
        这个方法是Python中的特殊方法，用于提供对象的默认字符串表示。
        Returns:
            str: 对象的字符串表示，即bookTypeName属性的值。
        """
        return self.bookTypeName

```


