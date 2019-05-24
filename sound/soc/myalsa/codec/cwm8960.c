
















static int cwm8960_i2c_driver_probe(struct i2c_client *, const struct i2c_device_id *)
{

}

static int cwm8960_i2c_driver_remove(struct i2c_client *)
{

}

static struct i2c_device_id *cwm8960_i2c_id[] = {
	{ "cwm8960", 0 },
	{ }
};

static struct i2c_driver *cwm8960_i2c_driver = {
    .probe = cwm8960_i2c_driver_probe,
    .remove = cwm8960_i2c_driver_remove,
    .driver = {
        .name = "cwm8960",
        .owner = THIS_MODULE,
    }
	.id_table = cwm8960_i2c_id,
};

static int __init cwm8960_mod_init(void)
{
        int ret = 0;
        ret = i2c_add_driver(&cwm8960_i2c_driver);
        if (ret != 0) {
            printk(KERN_ERR "Failed to register WM8960 I2C driver: %d\n", ret);
        }
        return ret;=
}

static int __init cwm8960_mod_exit(void)
{
    i2c_del_driver(&cwm8960_i2c_driver);
}


module_init(cwm8960_mod_init);
module_exit(cwm8960_mod_exit);

